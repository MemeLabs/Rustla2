import express from 'express';
import jwt from 'jwt-simple';
import process from 'process';
import Cookies from 'cookies';

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

// Returns private information. Requires the "jwt" cookie to contain a valid
// token.
api.get('/profile', async (req, res, next) => {
  const cookies = new Cookies(req, res);
  const token = cookies.get('jwt');

  // Unauthorized.
  if (!token) {
    res.sendStatus(401);
    return;
  }

  // Catch invalid token errors.
  try {
    const decoded = jwt.decode(token, process.env.JWT_SECRET);
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
