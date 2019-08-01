import React from 'react';
import { render } from 'react-dom';

import App from './components/App';
import store from './store';


const mountPoint = document.getElementById('main');
render(
  <App store={store} />,
  mountPoint,
  () => mountPoint.className = mountPoint.className.replace('jiggle-background', '')
);
