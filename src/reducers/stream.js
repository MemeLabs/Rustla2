import INITIAL_STATE from '../INITIAL_STATE';
import { actions } from '../actions/websocket';


const { STREAM_SET } = actions;

function streamReducer(state = INITIAL_STATE.stream, action) {
  switch (action.type) {
    case STREAM_SET: {
      const [stream] = action.payload;
      return stream.id;
    }
    default:
      return state;
  }
}

export default streamReducer;
