import INITIAL_STATE from '../INITIAL_STATE';
import { SET_CHAT_SIZE } from '../actions';


function uiReducer(state, action) {
  if (!state) {
    return INITIAL_STATE.ui;
  }
  switch (action.type) {
    case SET_CHAT_SIZE:
      return {
        ...state,
        chatSize: action.payload,
      };
  }
  return state;
}

export default uiReducer;
