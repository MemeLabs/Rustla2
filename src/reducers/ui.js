import INITIAL_STATE from '../INITIAL_STATE';
import {
  CHAT_HOST_STRIMS,
  SET_CHAT_SIZE,
  TOGGLE_CHAT,
  SHOW_CHAT,
} from '../actions';
import { actions } from '../actions/websocket';

const { STREAM_SET } = actions;

function uiReducer(state = INITIAL_STATE.ui, action) {
  switch (action.type) {
    case SET_CHAT_SIZE:
      return {
        ...state,
        chatSize: action.payload,
      };
    case STREAM_SET:
      return {
        ...state,
        chatHost: CHAT_HOST_STRIMS,
      };
    case TOGGLE_CHAT:
      return {
        ...state,
        chatHost: action.payload,
      };
    case SHOW_CHAT:
      return {
        ...state,
        showChat: action.payload,
      };
    default:
      return state;
  }
}

export default uiReducer;
