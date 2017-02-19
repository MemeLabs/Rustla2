import INITIAL_STATE from '../INITIAL_STATE';
import { LOGIN } from '../actions';


function loadingReducer(state = INITIAL_STATE.isLoading, action) {
  switch (action.type) {
    case LOGIN:
      return false;
  }
  return state;
}

export default loadingReducer;
