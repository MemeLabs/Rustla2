import * as React from 'react';
import { Route, Redirect, IndexRedirect, IndexRoute } from 'react-router';

import Strims from './components/Strims';
import Error404 from './components/Error404';


const routes =
  <Route path='/'>
    <IndexRoute component={Strims} />
    <Route path='strims' component={Strims} />
    <Route path='*' component={Error404} />
  </Route>
  ;

export default routes;
