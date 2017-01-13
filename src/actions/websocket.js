// TODO: remove `socket.io` and use native WebSockets once the new server is up
import io from 'socket.io-client';


const socket = io('https://api.overrustle.com/streams');

// the types of payloads we can expect from the server
const MSG_TYPES = [
  'error',
  'strims',
];

function init(store) {
  MSG_TYPES.forEach(type => {
    socket.on(type, payload => {
      // rename old server's "strims" to "streams"
      if (type === 'strims') {
        type = 'streams';
      }

      store.dispatch({ type, payload });
    });
  });
}

function emit(type, payload) {
  socket.emit(type, payload);
}

export { init, emit };
