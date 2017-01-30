import express from 'express';
import jwt from 'jwt-simple';
import process from 'process';

import { User } from '../db';
const debug = require('debug')('overrustle:api');

const api = express.Router();

api.get('/streamer/:name', async (req, res, next) => {
  try {
    const dbUser = await User.findById(req.params.name);
    res.json({
      service: dbUser.service,
      channel: dbUser.channel,
    });
  }
  catch (err) {
    return next(err);
  }
});

// Returns private information. Requires the "authorization" header with a valid
// JWT, like so:
//
//     Authorization: Bearer <token>
//
// See: https://en.wikipedia.org/wiki/JSON_Web_Token#How_it_works
api.get('/profile', async (req, res, next) => {
  const token = req.headers.authorization;

  // Unauthorized.
  if (!token) {
    res.sendStatus(401);
    return;
  }

  try {
    const decoded = jwt.decode(
      token.replace('Bearer ', ''),
      process.env.JWT_SECRET);
    res.status(200).json(decoded);
  }
  catch (e) {
    res.sendStatus(401);
  }
});

api.use((req, res) => {
  res.json({
    memes: true,
  });
});

export default api;
