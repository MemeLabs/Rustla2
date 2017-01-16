/* global __dirname process */
require('dotenv').config();

import 'babel-polyfill';
import http from 'http';
import path from 'path';
import express from 'express';
import morgan from 'morgan';
import WebSocket from 'uws';
import uuid from 'node-uuid';

import routes from './api';
import errors from './http_errors';


const debug = require('debug')('overrustle');
const app = express();

app.set('x-powered-by', false);
app.set('etag', false);
app.set('trust proxy', true);

app.use(morgan(':method :url :status :response-time ms - :res[content-length] - :remote-addr'));

app.use('/api', routes);
app.use(express.static(path.join(__dirname, '../public')));
app.use((req, res) => {
  res.sendFile(path.join(__dirname, '../public/index.html'));
});

app.use((err, req, res, next) => {
  if (!(err instanceof errors.HTTPError)) {
    const wrapper = new errors.InternalServerError(err.message);
    wrapper.error = err.name;
    wrapper.stack = err.stack;
    err = wrapper;
  }
  if (!err) {
    err = new errors.InternalServerError('unknown');
  }
  debug(err);
  res
    .status(err.status || 500)
    .json(err)
    .end()
    ;
});

// Websocket Stuff
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// interface Rustler {
//   stream: Stream | null; // the Stream that the user is watching
// }
// interface Stream {
//   id: uuidv4;
//   channel: string;
//   service: string;
//   overrustle: string | null;
//   thumbnail: string | null;
//   live: boolean;
//   rustlers: Set<websocket>; // people connected to the websocket server watching this stream
//   viewers: number; // amount of people the service reports is watching this stream
// }
const rustlers = new Map(); // websocket => Rustler map
const streams = new Map(); // `${channel}/${service}` => Stream map
// update all rustlers for this stream, and the lobby
const updateRustlers = (stream) => {
  for (const [ ws, rustler ] of rustlers.entries()) {
    if (rustler.stream === null || rustler.stream === stream) {
      ws.send(JSON.stringify(['RUSTLERS_UPDATE', stream.id, stream.rustlers.size]));
    }
  }
};
const wsEventHandlers = {
  // set the user's stream to [channel, service], or id, or null (for lobby)
  // eg, on client:
  // setStream('destiny', 'twitch');
  // setStream('fee55b7e-fac7-46b8-a5dd-4e86b106e846');
  // setStream(null); // lobby
  setStream(ws, channel, service) {
    const rustler = rustlers.get(ws);
    let stream;
    if (!channel) {
      stream = null;
    }
    else {
      if (!service) {
        for (const cached_stream of streams.values()) {
          if (cached_stream.id === channel) {
            stream = cached_stream;
            break;
          }
        }
      }
      else {
        stream = streams.get(`${channel}/${service}`);
      }
      if (!stream) {
        stream = {
          id: uuid.v4(),
          channel,
          service,
          overrustle: null,
          thumbnail: null,
          live: false,
          rustlers: new Set(),
          viewers: 0,
        };
      }
    }
    debug('rustler set stream %j => %j', rustler.stream, stream);
    // remove rustler from previous stream (if there was one)
    if (rustler.stream) {
      rustler.stream.rustlers.delete(rustler);
    }
    rustler.stream = stream;
    if (stream) {
      stream.rustlers.add(ws);
      streams.set(`${stream.channel}/${stream.service}`, stream);
    }
    updateRustlers(stream);
  },
  disconnect(ws) {
    const rustler = rustlers.get(ws);
    debug('rustler disconnect %j', rustler);
    if (rustler.stream) {
      rustler.stream.rustlers.delete(ws);
      updateRustlers(rustler.stream);
      rustler.stream = null;
    }
    rustlers.delete(ws);
  },
};

wss.on('connection', ws => {
  rustlers.set(ws, {
    stream: null,
  });
  ws.on('message', message => {
    try {
      const [ event, ...args ] = JSON.parse(message);
      const handler = wsEventHandlers[event];
      if (!handler) {
        throw new TypeError(`Invalid event "${event}"`);
      }
      handler(ws, ...args);
    }
    catch (err) {
      console.error(`Failed to handle incoming websocket message\n${message}\n`, err);
    }
  });
  ws.on('error', () => wsEventHandlers.disconnect(ws));
  ws.on('close', () => wsEventHandlers.disconnect(ws));
  ws.send(JSON.stringify([
    'STREAMS',
    Array.from(streams.values()).map(stream => ({
      ...stream,
      rustlers: stream.rustlers.size,
    })),
  ]));
});

server.listen(process.env.PORT || 80, () => debug(`listening on port ${process.env.PORT || 80}`));
