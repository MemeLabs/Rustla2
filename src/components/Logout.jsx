import React from 'react';
import { connect } from 'react-redux';
import { Redirect } from 'react-router';
import lifecycle from 'recompose/lifecycle';
import { compose } from 'redux';

import { logout } from '../actions';


const Logout = () => <Redirect to='/' />;

function mapDispatchToProps(dispatch) {
  return {
    logout() {
      dispatch(logout());
    },
  };
}

export default compose(
  connect(null, mapDispatchToProps),
  lifecycle({
    componentDidMount() {
      this.props.logout();
    },
  })
)(Logout);
