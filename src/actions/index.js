/* global API JWT_NAME SCALA_API */
import cookies from 'browser-cookies';
import { emit } from './websocket';


export const setStream = (...args) => () => {
  emit('setStream', ...args);
};

export const setAfk = (afk) => () => {
  emit('setAfk', afk);
};

export const STREAMER_FETCH = Symbol('STREAMER_FETCH');
export const STREAMER_FETCH_FAILURE = Symbol('STREAMER_FETCH_FAILURE');
export const fetchStreamer = name => async dispatch => {
  const res = await fetch(`${API}/streamer/${name}`);
  if (res.status !== 200) {
    const err = await res.json();
    return dispatch({
      type: STREAMER_FETCH_FAILURE,
      error: err,
    });
  }
  const streamer = await res.json();
  return dispatch({
    type: STREAMER_FETCH,
    payload: streamer,
  });
};


const CHAT_CLAMP_SIZE = 320;
export const SET_CHAT_SIZE = Symbol('SET_CHAT_SIZE');
export const setChatSize = size => dispatch => {
  // clamp our chat size a bit
  if (size < CHAT_CLAMP_SIZE) {
    size = CHAT_CLAMP_SIZE; // eslint-disable-line no-param-reassign
  }
  if (size > window.innerWidth - CHAT_CLAMP_SIZE) {
    size = window.innerWidth - CHAT_CLAMP_SIZE; // eslint-disable-line no-param-reassign
  }
  // save it in localStorage if supported
  if (localStorage) {
    localStorage.setItem('chatSize', size);
  }
  // dispatch the actual action
  dispatch({
    type: SET_CHAT_SIZE,
    payload: size,
  });
};


export const SET_PROFILE = Symbol('SET_PROFILE');
export const setProfile = profile => {
  return {
    type: SET_PROFILE,
    payload: profile,
  };
};


export const PROFILE_FETCH_START = Symbol('PROFILE_FETCH_START');
export const PROFILE_FETCH_FAILURE = Symbol('PROFILE_FETCH_FAILURE');
export const fetchProfile = (history) => async dispatch => {
  dispatch({
    type: PROFILE_FETCH_START,
    payload: undefined,
  });
  const res = await fetch(`${API}/profile`, {
    credentials: 'include',
  });
  if (res.status === 401 || res.status === 404) {
    return history.push('/login');
  }
  if (res.status !== 200) {
    const error = await res.json();
    return dispatch({
      type: PROFILE_FETCH_FAILURE,
      error,
    });
  }
  const profile = await res.json();
  return dispatch(setProfile(profile));
};


export const fetchProfileIfLoggedIn = () => async (dispatch, getState) => {
  if (!getState().self.isLoggedIn) {
    return;
  }
  return dispatch(fetchProfile());
};


export const PROFILE_UPDATE_FAILURE = Symbol('PROFILE_UPDATE_FAILURE');
export const updateProfile = profile => async dispatch => {
  dispatch({
    type: PROFILE_FETCH_START,
    payload: undefined,
  });
  const res = await fetch(`${API}/profile`, {
    method: 'POST',
    credentials: 'include',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(profile),
  });
  if (res.status !== 200) {
    const error = await res.json();
    return dispatch({
      type: PROFILE_UPDATE_FAILURE,
      error,
    });
  }
  const resProfile = await res.json();
  return dispatch(setProfile(resProfile));
};


export const LOGIN = Symbol('LOGIN');
export const login = () => dispatch => {
  const cookie = cookies.get(JWT_NAME);
  dispatch({
    type: LOGIN,
    payload: Boolean(cookie),
  });
};


export const LOGOUT = Symbol('LOGOUT');
export const logout = () => dispatch => {
  cookies.erase(JWT_NAME, {
    domain: `.${location.hostname}`,
  });
  dispatch({
    type: LOGOUT,
    payload: undefined,
  });
};

export const SHOW_HEADER = Symbol('SHOW_HEADER');
export const showHeader = value => dispatch => {
  dispatch({
    type: SHOW_HEADER,
    payload: value,
  });
};


export const SHOW_FOOTER = Symbol('SHOW_FOOTER');
export const showFooter = value => dispatch => {
  dispatch({
    type: SHOW_FOOTER,
    payload: value,
  });
};

export const SHOW_CHAT = Symbol('SHOW_CHAT');
export const showChat = value => dispatch => {
  if (localStorage) {
    localStorage.setItem('showChat', value);
  }

  dispatch({
    type: SHOW_CHAT,
    payload: value,
  });
};

export const TOGGLE_CHAT = Symbol('TOGGLE_CHAT');
export const CHAT_HOST_SERVICE = Symbol('CHAT_HOST_SERVICE');
export const CHAT_HOST_STRIMS = Symbol('CHAT_HOST_STRIMS');
export const CHAT_HOST_DGG = Symbol('CHAT_HOST_DGG');
export const toggleChat = host => dispatch => {
  dispatch(showChat(true));

  dispatch({
    type: TOGGLE_CHAT,
    payload: host,
  });
};


export const POLL_SET = Symbol('POLL_SET');
export const setPoll = poll => {
  return {
    type: POLL_SET,
    payload: poll,
  };
};

export const POLL_FETCH_START = Symbol('POLL_FETCH_START');
export const POLL_FETCH_FAILURE = Symbol('POLL_FETCH_FAILURE');
export const fetchPoll = (id) => async dispatch => {
  dispatch({
    type: POLL_FETCH_START,
    payload: {id},
  });
  const res = await fetch(`${SCALA_API}/v1/poll/${id}`, {
    credentials: 'include',
    headers: {
      'jwt': cookies.get(JWT_NAME),
    },
  });
  if (res.status !== 200) {
    const error = await res.json();
    return dispatch({
      type: POLL_FETCH_FAILURE,
      payload: {
        id,
        error,
      },
    });
  }
  const poll = await res.json();
  return dispatch(setPoll(poll));
};

export const POLL_CREATE_FAILURE = Symbol('POLL_CREATE_FAILURE');
export const createPoll = (poll, history) => async dispatch => {
  const res = await fetch(`${SCALA_API}/v1/poll`, {
    method: 'POST',
    credentials: 'include',
    headers: {
      'Content-Type': 'application/json',
      'jwt': cookies.get(JWT_NAME),
    },
    body: JSON.stringify(poll),
  });
  if (res.status !== 201) {
    const error = await res.json();
    return dispatch({
      type: POLL_CREATE_FAILURE,
      error,
    });
  }
  const resPoll = await res.json();
  history.push(`/poll/${resPoll.id}`);
  return dispatch(setPoll(resPoll));
};

export const POLL_VOTE_START = Symbol('POLL_VOTE_START');
export const POLL_VOTE_FAILURE = Symbol('POLL_VOTE_FAILURE');
export const submitPollVote = (id, options, history) => async dispatch => {
  dispatch({
    type: POLL_VOTE_START,
    id,
  });
  const res = await fetch(`${SCALA_API}/v1/poll/${id}/vote`, {
    method: 'POST',
    credentials: 'include',
    headers: {
      'Content-Type': 'application/json',
      'jwt': cookies.get(JWT_NAME),
    },
    body: JSON.stringify(options),
  });
  if (res.status < 200 || res.status >= 300) {
    const error = await res.json();
    return dispatch({
      type: POLL_VOTE_FAILURE,
      payload: {
        id,
        error,
      },
    });
  }
  history.push(`/poll/${id}/results`);
};

export const beginPollingPoll = id => dispatch => {
  let intervalId = setInterval(() => dispatch(fetchPoll(id)), 1000);

  return () => clearInterval(intervalId);
};
