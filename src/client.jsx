import React from 'react';
import { render } from 'react-dom';
import { Provider } from 'react-redux';
import { browserHistory, Router } from 'react-router';

import routes from './routes';
import store from './store';

const mountPoint = document.getElementById('main');
render(
  <Provider store={store}>
    <Router history={browserHistory}>
      {routes}
    </Router>
  </Provider>,
  mountPoint,
  () => mountPoint.className = mountPoint.className.replace('loading', '')
);
