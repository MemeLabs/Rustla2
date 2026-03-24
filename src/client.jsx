import React from 'react';
import { render } from 'react-dom';

import App from './components/App';
import store from './store';

const mountPoint = document.getElementById('main');

if (process.env.NODE_ENV !== 'production') {
  // used for testing redux dispatch events in console, since we're using symbols
  // they need to be attached to the window for them to match
  // otherwise it goes into a catchall(default)
  // you can now dispatch events using similar structure to line below
  // window.store.dispatch({type:window.types.IMAGE_MODAL_SRC, payload:"url"})
  import("./actions/index").then((types) => {
    window.store = store; 
    window.types = types;
  });
}

render(
  <App store={store} />,
  mountPoint,
  () => mountPoint.className = mountPoint.className.replace('jiggle-background', '')
);
