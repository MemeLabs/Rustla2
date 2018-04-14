import { CHAT_HOST_STRIMS } from './actions';

export default {
  isLoading: true,
  stream: null,
  streams: {},
  ui: {
    chatHost: CHAT_HOST_STRIMS,
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
