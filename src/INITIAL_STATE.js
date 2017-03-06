export default {
  isLoading: true,
  stream: null,
  streams: {},
  ui: {
    isOtherChatActive: false,
    chatSize: localStorage ? Number(localStorage.getItem('chatSize')) || 400 : 400,
  },
  self: {
    isLoggedIn: false,
    profile: {
      err: null,
      isFetching: false,
      data: null,
    },
  },
};
