import { combineReducers } from 'redux';

import profileReducer from './profile';
import streamReducer from './stream';
import streamsReducer from './streams';
import uiReducer from './ui';


export default combineReducers({
  profile: profileReducer,
  stream: streamReducer,
  streams: streamsReducer,
  ui: uiReducer,
});
