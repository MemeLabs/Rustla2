import INITIAL_STATE from '../INITIAL_STATE';
import { SET_PROFILE } from '../actions';


function profileReducer(state, action) {
  if (!state) {
    return INITIAL_STATE.profile;
  }
  switch (action.type) {
    case SET_PROFILE:
      return action.payload;
  }
  return state;
}

export default profileReducer;
