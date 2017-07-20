import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { redirectIfNotAdmin } from '../actions';
import MainLayout from './MainLayout';
import UserList from './UserList';


const Admin = ({ history }) => (
  <MainLayout history={history}>
    <div className='container'>
      <div className='page-header'>
        <h1>Admin</h1>
        <UserList />
      </div>
    </div>
  </MainLayout>
);

Admin.propTypes = {
  history: PropTypes.object.isRequired,
};

export default compose(
  connect(null, { redirectIfNotAdmin }),
  lifecycle({
    componentWillMount() {
      this.props.redirectIfNotAdmin(this.props.history);
    },
  }),
)(Admin);
