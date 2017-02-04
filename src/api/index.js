import express from 'express';
import jwt from 'jwt-simple';
import process from 'process';
import Cookies from 'cookies';

import { User } from '../db';
import errors from '../http_errors';


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
      const dbUser = await User.findById(decoded.sub);
      if (!dbUser) {
        throw new errors.NotFound();
      }
      res.json({
        username: dbUser.id,
        service: dbUser.service,
        channel: dbUser.channel,
      });
    }
    catch (e) {
      throw new errors.Unauthorized();
    }
  }
  catch (err) {
    return next(err);
  }
});

// Save changes to profile data. Returns updated profile data.
api.post('/profile', async (req, res, next) => {
  try {
    const cookies = new Cookies(req, res);
    const token = cookies.get('jwt');

    if (!token) {
      throw new errors.Unauthorized();
    }

    try {
      const decoded = jwt.decode(token, process.env.JWT_SECRET);
      const dbUser = await User.findById(decoded.sub);
      if (!dbUser) {
        throw new errors.NotFound();
      }
      dbUser.update(req.body);
      res.json({
        username: dbUser.id,
        service: dbUser.service,
        channel: dbUser.channel,
      });
    }
    catch (error) {
      throw new errors.Unauthorized();
    }
  }
  catch (error) {
    return next(error);
  }
});

api.use((req, res) => {
  res.json({
    memes: true,
  });
});

export default api;
