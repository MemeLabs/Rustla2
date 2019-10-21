/* global AFK_TIMEOUT */
import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import setPropTypes from 'recompose/setPropTypes';
import renderNothing from 'recompose/renderNothing';
import branch from 'recompose/branch';
import idx from 'idx';

import {
  setAfk,
  setStream,
  setChatSize,
  showChat,
  fetchProfileIfLoggedIn,
} from '../actions';

import IdleTimer from 'react-idle-timer';
import MainLayout from './MainLayout';
import Resizeable from './Resizeable';
import StreamEmbed from './StreamEmbed';
import ChatEmbed from './ChatEmbed';

import '../css/Stream';

export const Stream = ({
  chatClosed,
  history,
  service,
  channel,
  chatSize,
  setAfk,
  setChatSize,
  showChat,
  rustlerCount,
  showLeftChat = false,
}) => {
  let left = (
    <div className='flex-shrink-0 stream-embed' style={{ width: chatClosed ? '100%' : `calc(100% - ${chatSize}px)` }}>
      <StreamEmbed channel={channel} service={service} />
    </div>
  );
  let right = chatClosed ? null : (
    <div className='chat-embed' style={{ width: chatSize }}>
      <ChatEmbed onClose={() => showChat(false)} />
    </div>
  );
  if (showLeftChat) {
    const temp = left;
    left = right;
    right = temp;
  }
  return (
    <MainLayout history={history} showFooter={false} rustlerCount={rustlerCount}>
      <IdleTimer
        element={document}
        onActive={() => setAfk(false)}
        onIdle={() => setAfk(true)}
        timeout={AFK_TIMEOUT}
        />
      <Resizeable
        className='flex-grow-1 flex-column flex-lg-row'
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
  chatClosed: PropTypes.bool,
  history: PropTypes.object.isRequired,

  service: PropTypes.string.isRequired,
  channel: PropTypes.string.isRequired,

  chatSize: PropTypes.number.isRequired,
  showLeftChat: PropTypes.bool,

  setChatSize: PropTypes.func.isRequired,
  showChat: PropTypes.func.isRequired,
  rustlerCount: PropTypes.arrayOf(PropTypes.number),
  setAfk: PropTypes.func.isRequired,
};

export default compose(
  connect(
    state => ({
      chatSize: state.ui.chatSize,
      rustlerCount: state.streams[state.stream] ? [state.streams[state.stream].rustlers, state.streams[state.stream].viewers] : null,
      showLeftChat: idx(state, _ => _.self.profile.data.left_chat),
      isFetchingProfile: state.self.profile.isFetching,
      chatClosed: !state.ui.showChat,
    }),
    {
      setAfk,
      setStream,
      setChatSize,
      showChat,
      fetchProfileIfLoggedIn,
    },
  ),
  setPropTypes({
    streamer: PropTypes.string,
    service: PropTypes.string.isRequired,
    channel: PropTypes.string.isRequired,

    chatSize: PropTypes.number.isRequired,
    rustlerCount: PropTypes.arrayOf(PropTypes.number),
    showLeftChat: PropTypes.bool,

    setChatSize: PropTypes.func.isRequired,
    showChat: PropTypes.func.isRequired,
    setAfk: PropTypes.func.isRequired,
    setStream: PropTypes.func.isRequired,
  }),
  lifecycle({
    componentDidMount() {
      const { channel, service, streamer } = this.props;
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
