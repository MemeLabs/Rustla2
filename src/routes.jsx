/* global JWT_NAME */
import React from 'react';
import { Route, IndexRoute } from 'react-router';
import cookies from 'browser-cookies';

import Loading from './components/Loading';
import Streams from './components/Streams';
import Stream from './components/Stream';
import Error404 from './components/Error404';
import Profile from './components/Profile';

import { fetchStreamer, setProfile, login } from './actions';
import store from './store';
import INITIAL_STATE from './INITIAL_STATE';


const validServices = new Set(['angelthump', 'azubu', 'dailymotion', 'facebook', 'hitbox', 'hitbox-vod', 'mlg', 'nsfw-chaturbate', 'streamup', 'twitch', 'twitch-vod', 'ustream', 'vaughn', 'youtube', 'youtube-playlist']);

const routes =
  <Route path='/' component={Loading} onEnter={() => store.dispatch(login())}>
    <IndexRoute component={Streams} />
    <Route path='strims' component={Streams} />
    <Route path='profile' component={Profile} />
    <Route
      path='logout'
      onEnter={(nextState, replace) => {
        cookies.erase(JWT_NAME);
        store.dispatch(setProfile(INITIAL_STATE.profile));
        replace('/');
      }}
      />
    <Route
      path=':service/:channel'
      getComponent={(nextState, callback) => {
        const { service } = nextState.params;
        if (!validServices.has(service)) {
          return callback(null, Error404);
        }
        callback(null, Stream);
      }}
      />
    <Route
      path=':streamer'
      getComponent={async (nextState, callback) => {
        try {
          const { streamer } = nextState.params;
          if (!streamer) {
            throw new Error('`streamer` not specified by react-router');
          }
          const res = await store.dispatch(fetchStreamer(streamer));
          if (res.error) {
            throw res.error;
          }
          nextState.params.channel = res.payload.channel;
          nextState.params.service = res.payload.service;
          return callback(null, Stream);
        }
        catch (err) {
          console.error(err);
          return callback(null, Error404);
        }
      }}
      />
    <Route path='*' component={Error404} />
  </Route>
  ;

export default routes;
