const io = require('socket.io-client');

const msgTypes = [
  'error',
  'strims'
];

const socket = io('https://api.overrustle.com/streams');

function init(store) {
  msgTypes.forEach(type => {
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
  socket.send(payload);
}

export { init, emit };
