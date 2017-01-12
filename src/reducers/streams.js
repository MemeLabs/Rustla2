export default function streams(state = [], action) {
  if (action.type === 'streams') {
    state = action.payload.stream_list;
  }
  return state;
};
