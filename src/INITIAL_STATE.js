// @flow

import { CHAT_HOST_STRIMS } from './actions';

export default {
  isLoading: true,
  isAfk: false,
  polls: {},
  stream: null,
  streams: {},
  ui: {
    chatHost: CHAT_HOST_STRIMS,
    chatSize: localStorage ? Number(localStorage.getItem('chatSize')) || 400 : 400,
    showChat: localStorage ? !(localStorage.getItem('showChat') === 'false') : true,
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
