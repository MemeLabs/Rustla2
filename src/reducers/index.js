// @flow

import { combineReducers } from 'redux';

import selfReducer from './self';
import streamReducer from './stream';
import streamsReducer from './streams';
import uiReducer from './ui';
import loadingReducer from './loading';
import afkReducer from './afk';
import pollsReducer from './polls';


export default combineReducers({
  isLoading: loadingReducer,
  isAfk: afkReducer,
  polls: pollsReducer,
  stream: streamReducer,
  streams: streamsReducer,
  ui: uiReducer,
  self: selfReducer,
});
