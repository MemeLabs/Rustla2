import * as React from 'react';
import { Route, Redirect, IndexRedirect, IndexRoute } from 'react-router';

import App from './components/App';


const routes =
  <Route path='/' component={App}>
    <Route path='*' />
  </Route>
  ;

export default routes;
