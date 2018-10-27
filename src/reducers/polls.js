import INITIAL_STATE from '../INITIAL_STATE';
import {
  POLL_SET,
  POLL_FETCH_START,
  POLL_FETCH_FAILURE,
  POLL_CREATE_FAILURE,
} from '../actions';

function pollsReducer(state = INITIAL_STATE.polls, action) {
  switch (action.type) {
    case POLL_SET:
      return {
        ...state,
        [action.payload.id]: {
          loading: false,
          loaded: true,
          ...action.payload,
        },
      };
    case POLL_FETCH_START:
      return {
        ...state,
        [action.payload.id]: {
          loading: true,
          ...(state[action.payload.id] || {}),
        },
      };
    case POLL_FETCH_FAILURE:
      return {
        ...state,
        [action.payload.id]: {
          loading: false,
          error: action.error,
        },
      };
    case POLL_CREATE_FAILURE:
      return {
        ...state,
        create: {
          error: action.error,
        },
      };
    default:
      return state;
  }
}

export default pollsReducer;
