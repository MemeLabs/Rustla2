// @flow

declare var API_WS: string;
/* global process */

import WebSocket from 'reconnecting-websocket';
import type { Store } from 'redux';

import history from '../history';
import { setStream, setAfk } from '.';


let socket;
export let emit; // eslint-disable-line one-var

// the types of payloads we can expect from the server
export const actions = [
  'AFK_SET',
  'ERR',
  'RUSTLERS_SET',
  'STREAM_SET',
  'STREAMS_SET',
  'STREAM_GET',
  'STREAM_BANNED',
].reduce((acc, curr) => {
  acc[curr] = Symbol(curr);
  return acc;
}, {});

// thunks for these payloads
const thunks = {
  RUSTLERS_SET: payload => (dispatch, getState) => {
    const state = getState();
    const [ id ] = payload;
    if (!state.streams[id]) {
      emit('getStream', id);
    }
    else {
      dispatch({
        type: actions.RUSTLERS_SET,
        payload,
      });
    }
  },
  STREAM_BANNED: () => (dispatch) => {
    history.push('/beand');
    dispatch({
      type: actions.STREAM_BANNED,
      payload: null,
    });
  },
  STREAM_SET: (payload) => (dispatch, getState) => {
    dispatch({
      type: actions.STREAM_SET,
      payload,
    });
    if (getState().isAfk) {
      dispatch(setAfk(true));
    }
  },
};

export const init = (store: Store<*, *, *>) => {
  socket = new WebSocket(API_WS || `ws://${location.host}`);
  let messageQueue = [];
  let wasReconnect = false;
  emit = (...args) => {
    if (socket.readyState !== 1) {
      messageQueue.push(args);
    }
    else {
      socket.send(JSON.stringify(args));
    }
  };

  const ping = () => socket.send('');
  let pingInterval;

  socket.onopen = function onopen() {
    pingInterval = setInterval(ping, 20000);
    messageQueue.forEach(args => emit(...args));
    messageQueue = [];
    if (wasReconnect) {
      // resend setStream on reconnect if we're watching one
      const state = store.getState();
      const stream = state.streams[state.stream];
      if (stream) {
        if (stream.overrustle_id) {
          store.dispatch(setStream(stream.overrustle_id));
        }
        else {
          store.dispatch(setStream(stream.channel, stream.service));
        }
      }
    }
    wasReconnect = true;
  };

  socket.onclose = function onclose() {
    clearInterval(pingInterval);
  };

  socket.onmessage = function onmessage(event) {
    if (process.env.NODE_ENV !== 'production') {
      // eslint-disable-next-line no-console
      console.log('socket message', event);
    }

    const { data } = event;
    try {
      const [ ws_action, ...args ] = JSON.parse(data);
      const action = actions[ws_action];
      // make sure we know what to do with this action
      if (!action) {
        throw new TypeError(`Invalid action "${ws_action}"`);
      }
      // call any 'hook' thunks if they exist
      if (thunks[ws_action]) {
        store.dispatch(thunks[ws_action](args));
      }
      // just dispatch the action if no hooks exist
      else {
        store.dispatch({
          type: action,
          payload: args,
        });
      }
    }
    catch (err) {
      /* eslint-disable-next-line no-console */
      console.error(`Failed to handle incoming websocket action\n${data}\n`, err);
    }
  };
};

// expose this for testing
window.__emit__ = emit;
