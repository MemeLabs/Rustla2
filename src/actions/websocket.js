/* global API_WS */
import WebSocket from 'reconnecting-websocket';
import { browserHistory } from 'react-router';

import { setStream } from '.';


let socket;
export let emit; // eslint-disable-line one-var

// the types of payloads we can expect from the server
export const actions = [
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
    browserHistory.push('/beand');
    dispatch({
      type: actions.STREAM_BANNED,
      payload: null,
    });
  },
};

export const init = store => {
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

  socket.onopen = function onopen(event) {
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

  socket.onclose = function onclose(event) {
    clearInterval(pingInterval);
  };

  socket.onmessage = function onmessage(event) {
    // eslint-disable-next-line no-console
    console.log('socket message', event);

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
      console.error(`Failed to handle incoming websocket action\n${data}\n`, err);
    }
  };
};

// expose this for testing
window.__emit__ = emit;
