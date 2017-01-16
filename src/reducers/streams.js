import INITIAL_STATE from '../INITIAL_STATE';

export default function streams(state = INITIAL_STATE.streams, action) {
  if (action.type === 'streams') {
    state = action.payload.stream_list;
  }
  return state;
}
