// @flow

import { combineReducers } from 'redux';

import selfReducer from './self';
import streamReducer from './stream';
import streamsReducer from './streams';
import uiReducer from './ui';
import loadingReducer from './loading';
import afkReducer from './afk';


export default combineReducers({
  isLoading: loadingReducer,
  isAfk: afkReducer,
  stream: streamReducer,
  streams: streamsReducer,
  ui: uiReducer,
  self: selfReducer,
});
