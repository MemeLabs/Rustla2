import express from 'express';

import errors from '../http_errors';


const api = express.Router();

api.get('/streamer/:name', async (req, res, next) => {
  try {
    // TODO - fetch and send the streamer here
    throw new errors.NotFound();
  }
  catch (err) {
    return next(err);
  }
});

api.use((req, res) => {
  res.json({
    memes: true,
  });
});

export default api;
