require('dotenv').config();

import path from 'path';
import express from 'express';
import morgan from 'morgan';

import routes from './api';


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
  err = err || new Error('unknown');
  debug(err);
  res
    .status(500)
    .send(`Error: ${err.message && typeof err.message === 'string' ? err.message.trim() : 'unknown'}`)
    ;
});

app.listen(process.env.PORT || 80, () => debug(`listening on port ${process.env.PORT || 80}`));
