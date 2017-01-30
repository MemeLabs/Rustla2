export default {
  stream: null,
  streams: {},
  ui: {
    chatSize: localStorage ? Number(localStorage.getItem('chatSize')) || 400 : 400,
  },
};
