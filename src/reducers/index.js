import { combineReducers } from 'redux';
import { reducer as formReducer } from 'redux-form';

import streams from './streams';


const rootReducer = combineReducers({
  streams,
  form: formReducer,
});

export default rootReducer;
