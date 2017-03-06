import INITIAL_STATE from '../INITIAL_STATE';
import { SET_CHAT_SIZE, TOGGLE_CHAT } from '../actions';


function uiReducer(state = INITIAL_STATE.ui, action) {
  switch (action.type) {
    case SET_CHAT_SIZE:
      return {
        ...state,
        chatSize: action.payload,
      };
    case TOGGLE_CHAT:
      return {
        ...state,
        isOtherChatActive: action.payload,
      };
    default:
      return state;
  }
}

export default uiReducer;
