import express from 'express';
import sortBy from 'lodash/sortBy';
import { URL } from 'url';

import { Stream, User } from '../db';
import errors from '../http_errors';
const isValidAdvancedUrl = require('../util/is-valid-advanced-url')(URL);

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

    // Basic channel name sanitization
    let { channel, service } = req.body;
    if ((service !== 'advanced' && !/^[a-zA-Z0-9\-_]{1,64}$/.test(channel))
      || (service === 'advanced' && !isValidAdvancedUrl(channel))) {
      throw new errors.BadRequest('Invalid channel for the selected service');
    }

    // Encode any Unicode symbols in advanced channel URLs into a Punycode
    // string of ASCII symbols. This prevents users from cluttering up the
    // streams page with tons of emojis.
    if (service === 'advanced') {
      channel = new URL(channel).href;
    }

    await dbUser.update({
      service: req.body.service || dbUser.service,
      channel: channel || dbUser.channel,
      left_chat: req.body.hasOwnProperty('left_chat') && typeof req.body.left_chat === 'boolean' ? req.body.left_chat : dbUser.left_chat,
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
