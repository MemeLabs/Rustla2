import process from 'process';
import { createStore, applyMiddleware, compose } from 'redux';
import thunk from 'redux-thunk';
import createLogger from 'redux-logger';

import reducer from './reducers';
import INITIAL_STATE from './INITIAL_STATE';
import { init as wsInit, emit as wsEmit } from './actions/websocket';

const store = createStore(
  reducer,
  INITIAL_STATE,
  compose(
    applyMiddleware(
      thunk.withExtraArgument({ emit: wsEmit }),
      createLogger({
        duration: true,
        predicate: () => process.env.NODE_ENV === 'production',
        actionTransformer: action => ({
          ...action,
          type: String(action.type),
        }),
      }),
    ),
  ),
);

wsInit(store);

export default store;
