export default {
  stream: null,
  streams: {},
  ui: {
    chatSize: localStorage ? localStorage.getItem('chatSize') || 400 : 400,
  },
};
