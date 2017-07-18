import express from 'express';
import { isNil, sortBy } from 'lodash';

import { Stream, User } from '../db';
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
      left_chat: dbUser.left_chat,
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
    // basic channel name sanitization
    let channel = req.body.channel;
    if (!/^[a-zA-Z0-9\-_]{1,64}$/.test(channel)){
      channel = null;
    }
    await dbUser.update({
      service: req.body.service || dbUser.service,
      channel: channel || dbUser.channel,
      left_chat: !isNil(req.body.left_chat)
        ? req.body.left_chat
        : dbUser.left_chat,
    });
    res.json({
      username: dbUser.id,
      service: dbUser.service,
      channel: dbUser.channel,
      left_chat: dbUser.left_chat,
    });
  }
  catch (error) {
    return next(error);
  }
});

api.use(async (req, res) => {
  let streams = await Stream.findAllWithRustlers();
  streams = sortBy(streams, s => -s.rustlers);

  res.json({
    // Array of streams. Called "stream_list" to maintain backwards
    // compatibility with the old API (primarily for Bot).
    stream_list: streams.map(stream => {
      return {
        channel: stream.channel,
        live: stream.live,
        rustlers: stream.rustlers,
        service: stream.service,
        thumbnail: stream.thumbnail,

        // This begins with a forward slash because Bot expects it to.
        url: `/${stream.service}/${stream.channel}`,

        viewers: stream.viewers,
      };
    }),

    // Map of URL to rustler count. Redundant since this information exists
    // above, but this is used by bbdgg in the old API so it's here for
    // backwards compatibility.
    streams: streams.reduce((acc, stream) => {
      acc[`/${stream.service}/${stream.channel}`] = stream.rustlers;
      return acc;
    }, {}),
  });
});

export default api;
