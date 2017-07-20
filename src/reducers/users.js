import INITIAL_STATE from '../INITIAL_STATE';
import { GET_USERS, GET_USERS_FAILURE } from '../actions';


function uiReducer(state = INITIAL_STATE.users, action) {
  switch (action.type) {
    case GET_USERS:
      return action.payload;
    case GET_USERS_FAILURE:
      return INITIAL_STATE.users;
    default:
      return state;
  }
}

export default uiReducer;
