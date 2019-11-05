/* global AFK_TIMEOUT */
import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import setPropTypes from 'recompose/setPropTypes';
import renderNothing from 'recompose/renderNothing';
import branch from 'recompose/branch';

import {
  setAfk,
  setStream,
  fetchProfileIfLoggedIn,
  showFooter,
} from '../actions';

import IdleTimer from 'react-idle-timer';
import MainLayout from './MainLayout';
import StreamEmbed from './StreamEmbed';

import '../css/Stream';

export const Stream = ({
  history,
  service,
  channel,
  setAfk,
}) => {

  return (
    <MainLayout history={history} >
      <IdleTimer
        element={document}
        onActive={() => setAfk(false)}
        onIdle={() => setAfk(true)}
        timeout={AFK_TIMEOUT}
        />
        <StreamEmbed channel={channel} service={service} />
    </MainLayout>
  );
};

Stream.propTypes = {
  history: PropTypes.object.isRequired,

  service: PropTypes.string.isRequired,
  channel: PropTypes.string.isRequired,

  setAfk: PropTypes.func.isRequired,
};

export default compose(
  connect(
    state => ({
      isFetchingProfile: state.self.profile.isFetching,
    }),
    {
      setAfk,
      setStream,
      fetchProfileIfLoggedIn,
      showFooter,
    },
  ),
  setPropTypes({
    streamer: PropTypes.string,
    service: PropTypes.string.isRequired,
    channel: PropTypes.string.isRequired,

    setAfk: PropTypes.func.isRequired,
    setStream: PropTypes.func.isRequired,
  }),
  lifecycle({
    componentDidMount() {
      const { channel, service, streamer } = this.props;
      this.props.showFooter(false);
      if (streamer) {
        document.title = `${streamer} - Strims`;
        this.props.setStream(streamer);
        this.props.fetchProfileIfLoggedIn();
        return;
      }
      document.title = `${channel} on ${service} - Strims`;
      this.props.setStream(channel, service);
      this.props.fetchProfileIfLoggedIn();
      
    },

    // Catch updates to this component, which usually happen when the user goes
    // to a another stream after having been watching one already.
    componentDidUpdate(prevProps) {
      const { channel, service, streamer } = this.props;

      // Only dispatch action if user has navigated to a different stream.
      const hasChanged = (
        prevProps.channel !== channel ||
        prevProps.service !== service ||
        prevProps.streamer !== streamer
      );
      if (hasChanged) {
        if (streamer) {
          return this.props.setStream(streamer);
        }
        this.props.setStream(channel, service);
      }
    },
  }),
  branch(
    ({ isFetchingProfile }) => isFetchingProfile,
    renderNothing,
    Component => Component,
  ),
)(Stream);
