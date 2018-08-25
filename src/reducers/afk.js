import INITIAL_STATE from '../INITIAL_STATE';
import { actions } from '../actions/websocket';

const { AFK_SET } = actions;


function afkReducer(state = INITIAL_STATE.isLoading, action) {
  switch (action.type) {
    case AFK_SET:
      return action.payload[0];
  }
  return state;
}

export default afkReducer;
