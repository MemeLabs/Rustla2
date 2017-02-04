import React, { PropTypes } from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';

import { updateProfile } from '../actions';
import MainLayout from './MainLayout';
import ServiceSelect from './ServiceSelect';

import '../css/Profile';


const Profile = ({ profile, updateProfile }) =>
  <MainLayout>
    <div className='profile'>
      <h1>Settings</h1>
      <form
        className='form-horizontal'
        onSubmit={event => {
          event.preventDefault();
          const payload = {
            ...profile,
            service: event.target.elements.service.value,
            channel: event.target.elements.channel.value,
          };

          // Disallow blank inputs. The ORM will disallow these anyways but
          // there's arguably no point in even making the request if it's known
          // ahead of time that it's gonna fail. This check might be better off
          // in the `updateProfile` action itself. Also, would be nice if we
          // visually showed the user an error message regarding this (or simply
          // disabled the button).
          if (!payload.channel || !payload.channel.length) {
            return;
          }

          updateProfile(payload);
        }}
      >
        <div className='form-group'>
          <label
            htmlFor='profile-service-select'
            className='col-sm-2 control-label'>Streaming Service</label>
          <div className='col-sm-10'>
            <ServiceSelect id='profile-service-select' />
          </div>
        </div>
        <div className='form-group'>
          <label
            htmlFor='profile-channel'
            className='col-sm-2 control-label'>Channel/Video ID</label>
          <div className='col-sm-10'>
            <input
              className='form-control'
              id='profile-channel'
              type='text'
              name='channel'
              placeholder={profile.channel}
              />
          </div>
        </div>
        <button type='submit' className='btn btn-primary'>Save Changes</button>
      </form>
    </div>
  </MainLayout>
  ;

Profile.propTypes = {
  profile: PropTypes.shape({
    username: PropTypes.string.isRequired,
    service: PropTypes.string.isRequired,
    channel: PropTypes.string.isRequired,
  }),
};

export default compose(
  connect(
    state => ({
      profile: state.profile,
    }),
    {
      updateProfile,
    }
  ),
)(Profile);
