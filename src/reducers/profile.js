import INITIAL_STATE from '../INITIAL_STATE';
import { SET_PROFILE } from '../actions';


function profileReducer(state = INITIAL_STATE.profile, action) {
  switch (action.type) {
    case SET_PROFILE:
      return action.payload;
    default:
      return state;
  }
}

export default profileReducer;
