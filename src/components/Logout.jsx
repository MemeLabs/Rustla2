// @flow

import React from 'react';
import { connect } from 'react-redux';
import { Redirect } from 'react-router';
import lifecycle from 'recompose/lifecycle';
import { compose } from 'redux';
import type { Dispatch } from 'redux';

import { logout } from '../actions';

type OwnProps = {||};
type Props = {|
  ...OwnProps,
  +logout: () => void
|};

const Logout = () => <Redirect to='/' />;

function mapDispatchToProps(dispatch: Dispatch<*>): $Shape<Props> {
  return {
    logout() {
      dispatch(logout());
    },
  };
}

export default compose(
  connect<Props, OwnProps, _, _, _, _>(null, mapDispatchToProps),
  lifecycle({
    componentDidMount() {
      this.props.logout();
    },
  })
)(Logout);
