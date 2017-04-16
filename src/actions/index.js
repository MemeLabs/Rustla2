/* global API JWT_NAME */
import { browserHistory } from 'react-router';
import cookies from 'browser-cookies';
import { emit } from './websocket';


export const setStream = (...args) => () => {
  emit('setStream', ...args);
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


export const SET_CHAT_SIZE = Symbol('SET_CHAT_SIZE');
export const setChatSize = size => dispatch => {
  // clamp our chat size a bit
  const CLAMP_SIZE = 320;
  if (size < CLAMP_SIZE) {
    size = CLAMP_SIZE;
  }
  if (size > window.innerWidth - CLAMP_SIZE) {
    size = window.innerWidth - CLAMP_SIZE;
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
export const fetchProfile = () => async dispatch => {
  dispatch({
    type: PROFILE_FETCH_START,
    payload: undefined,
  });
  const res = await fetch(`${API}/profile`, {
    credentials: 'include',
  });
  if (res.status === 401 || res.status === 404) {
    return browserHistory.push('/login');
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

export const TOGGLE_CHAT = Symbol('TOGGLE_CHAT');
export const toggleChat = isOtherChatActive => {
  return {
    type: TOGGLE_CHAT,
    payload: isOtherChatActive,
  };
};

export const STREAM_BANNED = Symbol('STREAM_BANNED');
export const streamBanned = () => dispatch => {
  //browserHistory.push('/');
  alert('stream banned or something');
  dispatch({
    type: STREAM_BANNED,
    payload: null,
  });
};
