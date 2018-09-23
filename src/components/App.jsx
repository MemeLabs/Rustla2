import React from 'react';
import PropTypes from 'prop-types';
import { connect, Provider } from 'react-redux';
import { Router } from 'react-router-dom';
import { compose } from 'redux';
import { lifecycle } from 'recompose';

import { login } from '../actions';
import Routes from '../routes';
import history from '../history';

const App = ({ store }) =>
  <Provider store={store}>
    <Router history={history}>
      <Routes />
    </Router>
  </Provider>;

App.propTypes = {
  store: PropTypes.object.isRequired,
};

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
