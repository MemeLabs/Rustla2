/* global __dirname process */
require('dotenv').config();

import 'babel-polyfill';
import http from 'http';
import path from 'path';
import express from 'express';
import morgan from 'morgan';
import WebSocket from 'uws';
import uuid from 'uuid/v4';

import routes from './api';
import errors from './http_errors';
import getThumbnail from './get-thumbnail';


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
const updateRustlers = stream => {
  // ensure we're passing a stream whose viewers we will be notifying
  if (!stream) {
    return;
  }
  for (const [ ws, rustler ] of rustlers.entries()) {
    // send this update to everyone on this stream and everyone in the lobby
    if (rustler.stream === null || rustler.stream === stream) {
      ws.send(JSON.stringify(['RUSTLERS_SET', stream.id, stream.rustlers.size]));
    }
  }
};

const wsEventHandlers = {
  // get information about a stream
  // getStream('fee55b7e-fac7-46b8-a5dd-4e86b106e846');
  getStream(ws, id) {
    for (const stream of streams.values()) {
      if (stream.id === id) {
        ws.send(JSON.stringify([
          'STREAM_GET',
          {
            ...stream,
            rustlers: stream.rustlers.size,
          },
        ]));
        return;
      }
    }
  },

  // set the user's stream to [channel, service], or id, or null (for lobby)
  // eg, on client:
  // setStream('destiny', 'twitch');
  // setStream('fee55b7e-fac7-46b8-a5dd-4e86b106e846');
  // setStream(null); // lobby
  async setStream(ws, channel, service) {
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
          if (!stream) {
            throw new Error(`unknown stream id "${channel}"`);
          }
        }
      }
      else {
        stream = streams.get(`${channel}/${service}`);
      }
      if (!stream) {
        stream = {
          id: uuid(),
          channel,
          service,
          overrustle: null,
          thumbnail: null,
          live: false,
          rustlers: new Set(),
          viewers: 0,
        };

        // Twitch API v5: must convert channel name to numeric ID
        if (service === 'twitch') {
          const response =
            await fetch(`https://api.twitch.tv/kraken/users?login=${channel}`,
              {
                headers: {
                  Accept: 'application/vnd.twitchtv.v5+json',
                  'Client-ID': process.env.TWITCH_CLIENT_ID
                }
              });
          const data = await response.json();

          // TODO: figure out why Twitch returns an array of users
          stream.twitch_channel_id = parseInt(data.users[0]._id, 10);
        }
        stream.thumbnail = await getThumbnail(stream);
      }
    }
    debug('rustler set stream %j => %j', rustler.stream, stream);
    // remove rustler from previous stream (if there was one)
    let prevStream = null;
    if (rustler.stream) {
      prevStream = rustler.stream;
      rustler.stream.rustlers.delete(rustler);
    }
    rustler.stream = stream;
    if (stream) {
      stream.rustlers.add(ws);
      streams.set(`${stream.channel}/${stream.service}`, stream);
      ws.send(JSON.stringify(['STREAM_SET', {
        ...stream,
        rustlers: stream.rustlers.size,
      }]));
    }
    else {
      ws.send(JSON.stringify([
        'STREAMS_SET',
        Array.from(streams.values()).map(stream => ({
          ...stream,
          rustlers: stream.rustlers.size,
        })),
      ]));
    }
    updateRustlers(stream || prevStream);
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
      ws.send(JSON.stringify([
        'ERR',
        err.message,
      ]));
    }
  });
  ws.on('error', () => wsEventHandlers.disconnect(ws));
  ws.on('close', () => wsEventHandlers.disconnect(ws));
});

setInterval(() => {
  console.log(rustlers);
  console.log(streams);
  console.log('=======================================================================');
}, 10000);

server.listen(process.env.PORT || 80, () => debug(`listening on port ${process.env.PORT || 80}`));
