import express from 'express';
import jwt from 'jwt-simple';
import process from 'process';
import Cookies from 'cookies';

import { User } from '../db';
import errors from '../http_errors';
const debug = require('debug')('overrustle:api');

const api = express.Router();

api.get('/streamer/:name', async (req, res, next) => {
  try {
    const dbUser = await User.findById(req.params.name);
    if (!dbUser) {
      throw new errors.NotFound();
    }
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
  try {
    const cookies = new Cookies(req, res);
    const token = cookies.get('jwt');

    // Unauthorized.
    if (!token) {
      throw new errors.Unauthorized();
    }

    // Catch invalid token errors.
    try {
      const decoded = jwt.decode(token, process.env.JWT_SECRET);
      res.status(200).json(decoded);
    }
    catch (e) {
      throw new errors.Unauthorized();
    }
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
