import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { getUsers } from '../actions';


const UserList = ({ users }) => (
  <table className='table'>
    <thead>
      <tr>
        <th>username</th>
        <th>service</th>
        <th>channel</th>
      </tr>
    </thead>
    <tbody>
      {
        users.map((user) => (
          <tr key={user.username}>
            <td>{user.username}</td>
            <td>{user.service}</td>
            <td>{user.channel}</td>
          </tr>
        ))
      }
    </tbody>
  </table>
);

UserList.propTypes = {
  users: PropTypes.array.isRequired,
};

export default compose(
  connect((state) => ({ users: state.users }), { getUsers }),
  lifecycle({
    componentDidMount() {
      this.props.getUsers();
    },
  }),
)(UserList);
