import express from 'express';

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

// Returns private information. Requires the "jwt" cookie to contain a valid token.
api.get('/profile', async (req, res, next) => {
  try {
    // Unauthorized.
    if (!req.session) {
      throw new errors.Unauthorized();
    }

    const dbUser = await User.findById(req.session.id);
    if (!dbUser) {
      throw new errors.NotFound();
    }
    res.json({
      username: dbUser.id,
      service: dbUser.service,
      channel: dbUser.channel,
    });
  }
  catch (err) {
    return next(err);
  }
});

// Save changes to profile data. Returns updated profile data.
api.post('/profile', async (req, res, next) => {
  try {
    if (!req.session) {
      throw new errors.Unauthorized();
    }

    const dbUser = await User.findById(req.session.id);
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
    return next(error);
  }
});

api.use((req, res) => {
  res.json({
    memes: true,
  });
});

export default api;
