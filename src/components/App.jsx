import React from 'react';
import { connect, Provider } from 'react-redux';
import { Router } from 'react-router-dom';
import { compose } from 'redux';
import lifecycle from 'recompose/lifecycle';

import { login } from '../actions';
import Routes from '../routes';
import history from '../history';


const App = ({ store }) => (
  <Provider store={store}>
    <Router history={history}>
      <Routes />
    </Router>
  </Provider>
);

export default compose(
  connect(
    null,
    {
      login,
    },
  ),
  lifecycle({
    componentDidMount() {
      this.props.login();
    },
  }),
)(App);
