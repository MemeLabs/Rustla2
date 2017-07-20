import { combineReducers } from 'redux';

import selfReducer from './self';
import streamReducer from './stream';
import streamsReducer from './streams';
import uiReducer from './ui';
import loadingReducer from './loading';
import usersReducer from './users';


export default combineReducers({
  isLoading: loadingReducer,
  stream: streamReducer,
  streams: streamsReducer,
  ui: uiReducer,
  self: selfReducer,
  users: usersReducer,
});
