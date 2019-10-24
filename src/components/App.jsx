import React from 'react';
import PropTypes from 'prop-types';
import { connect, Provider } from 'react-redux';
import { compose } from 'redux';
import { lifecycle } from 'recompose';

import { login } from '../actions';
import RoutesWithChat from './RoutesWithChat';

const App = ({ store }) =>
  <Provider store={store}>
    <RoutesWithChat />
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
