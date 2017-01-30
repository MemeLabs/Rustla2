/* global __dirname */
require('dotenv').config();

import 'babel-polyfill';
import 'isomorphic-fetch';
import Cookies from 'cookies';
import http from 'http';
import path from 'path';
import process from 'process';
import querystring from 'querystring';
import express from 'express';
import morgan from 'morgan';
import jwt from 'jwt-simple';

import routes from './api';
import errors from './http_errors';
import makeWebSocketServer from './api/websocket';
import { User } from './db';

const debug = require('debug')('overrustle');
const app = express();

app.set('x-powered-by', false);
app.set('etag', false);
app.set('trust proxy', true);

app.use(morgan(':method :url :status :response-time ms - :res[content-length] - :remote-addr'));

app.use('/api', routes);

// send user to authenticate their Twitch account
app.use('/login', (req, res) => {
  const qs = querystring.stringify({
    response_type: 'code',
    client_id: process.env.TWITCH_CLIENT_ID,
    redirect_uri: process.env.TWITCH_REDIRECT_URI,
    scope: 'user_read',
  });
  res.redirect(`https://api.twitch.tv/kraken/oauth2/authorize?${qs}`);
});

// Twitch will then redirect user to this path, with a code that we can use to
// obtain the access token.
app.use('/oauth', async (req, res, next) => {
  if (req.query.error) {
    switch (req.query.error) {
      case 'redirect_mismatch':
        console.error(`
          Got "redirect mismatch" error from Twitch. Please ensure that you have
          set the TWITCH_REDIRECT_URI environment variable to the redirect URI
          defined in your Twitch application.
        `);
        break;
      default:
        debug('got error from twitch authentification: ', req.query);
    }
    next();
    return;
  }

  const oauthRequest = await fetch('https://api.twitch.tv/kraken/oauth2/token', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      client_id: process.env.TWITCH_CLIENT_ID,
      client_secret: process.env.TWITCH_CLIENT_SECRET,
      grant_type: 'authorization_code',
      redirect_uri: process.env.TWITCH_REDIRECT_URI,
      code: req.query.code,
    }),
  });
  const oauthResult = await oauthRequest.json();

  // This failure is almost certainly caused by the TWITCH_CLIENT_SECRET
  // environment variable not being defined.
  if (oauthResult.status === 400) {
    debug(oauthResult);
    res.redirect('/');
  }

  // Now, use the user access token in order to get the Twitch username.
  const getUserRequest = await fetch('https://api.twitch.tv/kraken/user', {
    method: 'GET',
    headers: {
      Accept: 'application/json',
      Authorization: `OAuth ${oauthResult.access_token}`,
      'Client-ID': process.env.TWITCH_CLIENT_ID,
    },
  });
  const getUserResult = await getUserRequest.json();

  let dbUser = await User.findById(getUserResult.name);
  if (!dbUser) {
    debug(`user with Twitch username ${getUserResult.name} does not exist,
      creating new one`);
    dbUser = await User.create({
      id: getUserResult.name,
      service: 'twitch',
      channel: getUserResult.name,
      last_ip: req.connection.remoteAddress,
      last_seen: new Date(),
    });
  }
  else {
    dbUser.update({
      last_ip: req.connection.remoteAddress,
      last_seen: new Date(),
    });
  }

  const payload = {
    sub: getUserResult.name,
  };

  const token = jwt.encode(payload, process.env.JWT_SECRET);
  const cookies = new Cookies(req, res);
  cookies.set('jwt', token, {
    expires: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000),
  });

  res.redirect('/profile');
});

app.use('/profile', (req, res, next) => {
  const cookies = new Cookies(req, res);

  // Catch decode failure (e.g., invalid payload or signature).
  let token;
  try {
    token = jwt.decode(cookies.get('jwt'), process.env.JWT_SECRET);
  }
  catch (e) {
    debug('got error decoding JWT: ', e.message);
    res.redirect('/');
    return;
  }

  next();
});

app.use(express.static(path.join(__dirname, '../public')));
app.use((req, res) => {
  res.sendFile(path.join(__dirname, '../public/index.html'));
});

app.use((err, req, res) => {
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
