/* global __dirname process */
require('dotenv').config();

import 'babel-polyfill';
import http from 'http';
import path from 'path';
import express from 'express';
import morgan from 'morgan';

import routes from './api';
import errors from './http_errors';
import makeWebSocketServer from './api/websocket';


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

const server = http.createServer(app);
makeWebSocketServer(server);

server.listen(process.env.PORT || 80, () => debug(`listening on port ${process.env.PORT || 80}`));
