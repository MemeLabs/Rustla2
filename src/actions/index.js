/* global API */

import { emit } from './websocket';


export const setStream = (...args) => () => {
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
export const setChatSize = size => (dispatch) => {
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

export const SET_PROFILE = Symbol('SET_PROFILE');
export const setProfile = profile => {
  return {
    type: SET_PROFILE,
    payload: profile,
  };
};

export const PROFILE_FETCH_FAILURE = Symbol('PROFILE_FETCH_FAILURE');
export const fetchProfile = () => async (dispatch) => {
  const res = await fetch(`${API}/profile`, {
    credentials: 'same-origin',
  });
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

export const PROFILE_UPDATE_FAILURE = Symbol('PROFILE_UPDATE_FAILURE');
export const updateProfile = (profile) => async (dispatch) => {
  const res = await fetch(`${API}/profile`, {
    method: 'POST',
    credentials: 'same-origin',
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
