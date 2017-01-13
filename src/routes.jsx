import React from 'react';
import { Route, IndexRoute } from 'react-router';

import Strims from './components/Strims';
import Strim from './components/Strim';
import Error404 from './components/Error404';


const routes =
  <Route path='/'>
    <IndexRoute component={Strims} />
    <Route path='strims' component={Strims} />
    <Route path=':service/:channel' component={Strim} />
    <Route path='*' component={Error404} />
  </Route>
  ;

export default routes;
