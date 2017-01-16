/* global API_WS */

import store from './store';


const socket = new WebSocket(API_WS || `ws://${location.host}`);

// // the types of payloads we can expect from the server
// const MSG_TYPES = [
//   'error',
//   'strims',
// ];

// function init(store) {
//   MSG_TYPES.forEach(type => {
//     socket.on(type, payload => {
//       // rename old server's "strims" to "streams"
//       if (type === 'strims') {
//         type = 'streams';
//       }
//
//       store.dispatch({ type, payload });
//     });
//   });
// }


const messageQueue = [];
export const emit = (...args) => {
  if (socket.readyState !== 1) {
    messageQueue.push(args);
  }
  else {
    socket.send(JSON.stringify(args));
  }
};

socket.onopen = function onopen(event) {
  console.log('socket opened', event);
  messageQueue.forEach(args => emit(...args));
};

export const actions = [
  'RUSTLERS_UPDATE',
].reduce((acc, curr) => {
  acc[curr] = Symbol(curr);
  return acc;
}, {});

socket.onmessage = function onmessage(event) {
  console.log('socket message', event);
  const { data } = event;
  try {
    const [ ws_action, ...args ] = JSON.parse(data);
    const action = actions[ws_action];
    if (!action) {
      throw new TypeError(`Invalid action "${ws_action}"`);
    }
    store.dispatch({
      type: action,
      payload: args,
    });
  }
  catch (err) {
    console.error(`Failed to handle incoming websocket action\n${data}\n`, err);
  }
};

window.__emit__ = function emit(...args) {
  emit(...args);
};
