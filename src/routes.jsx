import React from 'react';
import { Route, IndexRoute } from 'react-router';

import Streams from './components/Streams';
import Stream from './components/Stream';
import Error404 from './components/Error404';


const routes =
  <Route path='/'>
    <IndexRoute component={Streams} />
    <Route path='strims' component={Streams} />
    <Route path=':service/:channel' component={Stream} />
    <Route path='*' component={Error404} />
  </Route>
  ;

export default routes;
