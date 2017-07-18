import React from 'react';
import { connect, Provider } from 'react-redux';
import { BrowserRouter } from 'react-router-dom';
import { compose } from 'redux';
import lifecycle from 'recompose/lifecycle';

import { login } from '../actions';
import Routes from '../routes';


const App = ({ store }) => (
  <Provider store={store}>
    <BrowserRouter>
      <Routes />
    </BrowserRouter>
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
