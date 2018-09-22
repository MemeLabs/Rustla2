import omit from 'lodash/omit';

import INITIAL_STATE from '../INITIAL_STATE';
import { actions } from '../actions/websocket';


const {
  STREAM_GET,
  STREAM_SET,
  RUSTLERS_SET,
  STREAMS_SET,
} = actions;

function streamsReducer(state = INITIAL_STATE.streams, action) {
  switch (action.type) {
    case STREAM_GET:
    case STREAM_SET: {
      const [ stream ] = action.payload;
      if (stream) {
        return {
          ...state,
          [stream.id]: stream,
        };
      }
      return state;
    }
    case RUSTLERS_SET: {
      const [ id, rustlers ] = action.payload;

      // Flush this stream if there are no viewers.
      if (!rustlers) {
        return omit(state, [id]);
      }

      return {
        ...state,
        [id]: {
          ...state[id],
          id,
          rustlers,
        },
      };
    }
    case STREAMS_SET: {
      const [ streams ] = action.payload;
      return {
        ...streams.reduce((acc, stream) => {
          acc[stream.id] = stream;
          return acc;
        }, {}),
      };
    }
    default:
      return state;
  }
}

export default streamsReducer;
