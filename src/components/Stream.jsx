import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import setPropTypes from 'recompose/setPropTypes';
import renderNothing from 'recompose/renderNothing';
import branch from 'recompose/branch';
import get from 'lodash/get';

import { setStream, setChatSize, fetchProfileIfLoggedIn } from '../actions';

import MainLayout from './MainLayout';
import Resizeable from './Resizeable';
import StreamEmbed from './StreamEmbed';
import ChatEmbed from './ChatEmbed';


export const Stream = ({ params: { channel, service }, chatSize, setChatSize, rustlerCount, showLeftChat = false }) => {
  let left = (
    <div style={{ width: `calc(100% - ${chatSize}px)` }}>
      <StreamEmbed channel={channel} service={service} />
    </div>
  );
  let right = (
    <div style={{ width: chatSize }}>
      <ChatEmbed />
    </div>
  );
  if (showLeftChat) {
    const temp = left;
    left = right;
    right = temp;
  }
  return (
    <MainLayout showFooter={false} rustlerCount={rustlerCount}>
      <Resizeable
        className='grow-1'
        onResize={e => {
          let newChatSize;
          if (showLeftChat) {
            newChatSize = e.pageX;
          }
          else {
            newChatSize = window.innerWidth - e.pageX;
          }
          setChatSize(newChatSize);
        }}
        >
        {left}
        {right}
      </Resizeable>
    </MainLayout>
  );
};

Stream.propTypes = {
  params: PropTypes.shape({
    channel: PropTypes.string.isRequired,
    service: PropTypes.string.isRequired,
  }),

  chatSize: PropTypes.number.isRequired,
  showLeftChat: PropTypes.bool,

  setChatSize: PropTypes.func.isRequired,
  rustlerCount: MainLayout.propTypes.rustlerCount,
};

export default compose(
  connect(
    state => ({
      chatSize: state.ui.chatSize,
      rustlerCount: state.streams[state.stream] ? [state.streams[state.stream].rustlers, state.streams[state.stream].viewers] : null,
      showLeftChat: get(state, ['self', 'profile', 'data', 'left_chat']),
      isFetchingProfile: state.self.profile.isFetching,
    }),
    {
      setStream,
      setChatSize,
      fetchProfileIfLoggedIn,
    },
  ),
  setPropTypes({
    params: PropTypes.shape({
      channel: PropTypes.string.isRequired,
      service: PropTypes.string.isRequired,
    }),

    chatSize: PropTypes.number.isRequired,
    rustlerCount: MainLayout.propTypes.rustlerCount,
    showLeftChat: PropTypes.bool.isRequired,

    setChatSize: PropTypes.func.isRequired,
    setStream: PropTypes.func.isRequired,
  }),
  lifecycle({
    componentDidMount() {
      const { channel, service, streamer } = this.props.params;
      if (streamer) {
        return this.props.setStream(streamer);
      }
      this.props.setStream(channel, service);
      this.props.fetchProfileIfLoggedIn();
    },

    // Catch updates to this component, which usually happen when the user goes
    // to a another stream after having been watching one already.
    componentDidUpdate(prevProps) {
      const { channel, service, streamer } = this.props.params;

      // Only dispatch action if user has navigated to a different stream.
      const hasChanged = (
        prevProps.params.channel !== channel ||
        prevProps.params.service !== service ||
        prevProps.params.streamer !== streamer
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
