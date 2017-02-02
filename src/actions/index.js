/* global API */
import cookies from 'js-cookie';

import { emit } from './websocket';

export const setStream = (...args) => (dispatch, getState) => {
  emit('setStream', ...args);
};

export const STREAMER_FETCH = Symbol('STREAMER_FETCH');
export const STREAMER_FETCH_FAILURE = Symbol('STREAMER_FETCH_FAILURE');
export const fetchStreamer = (name) => async (dispatch) => {
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
export const setChatSize = size => (dispatch, getState) => {
  // clamp our chat size a bit
  if (size < 320) {
    size = 320;
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

export const PROFILE_FETCH = Symbol('PROFILE_FETCH');
export const PROFILE_FETCH_FAILURE = Symbol('PROFILE_FETCH_FAILURE');
export const fetchProfile = () => async (dispatch) => {
  const token = cookies.get('jwt');
  const res = await fetch(`${API}/profile`, {
    credentials: 'same-origin',
    headers: {
      Authorization: `Bearer ${token}`,
    },
  });
  if (res.status !== 200) {
    const error = await res.json();
    return dispatch({
      type: PROFILE_FETCH_FAILURE,
      error,
    });
  }
  const profile = await res.json();
  return dispatch({
    type: PROFILE_FETCH,
    payload: profile,
  });
};
