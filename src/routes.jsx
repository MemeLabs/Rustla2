// @flow

import React from 'react';
import { Route, Switch } from 'react-router-dom';

import AsyncProfile from './components/AsyncProfile';
import AsyncStream from './components/AsyncStream';
import AsyncStreams from './components/AsyncStreams';
import Banned from './components/Banned';
import Error404 from './components/Error404';
import Logout from './components/Logout';
import PollCreate from './components/PollCreate';
import PollVote from './components/PollVote';
import PollResult from './components/PollResult';


const Routes = () =>
  <div className='routes'>
    <Switch>
      <Route exact path='/' component={AsyncStreams} />
      <Route path='/strims' component={AsyncStreams} />
      <Route path='/profile' component={AsyncProfile} />
      <Route path='/login' component={() => window.location.reload()} />
      <Route path='/logout' component={Logout} />
      <Route path='/beand' component={Banned} />
      <Route path='/poll/create' component={PollCreate} />
      <Route path='/poll/:poll' component={PollVote} exact />
      <Route path='/poll/:poll/results' component={PollResult} />
      <Route path='/:service/:channel(.+)' component={AsyncStream} />
      <Route path='/:streamer' component={AsyncStream} />
      <Route component={Error404} />
    </Switch>
  </div>
  ;

export default Routes;
