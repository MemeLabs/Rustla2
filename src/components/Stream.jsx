import React, { PropTypes } from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import setPropTypes from 'recompose/setPropTypes';

import { setStream, setChatSize } from '../actions';

import MainLayout from './MainLayout';
import Resizeable from './Resizeable';
import StreamEmbed from './StreamEmbed';
import ChatEmbed from './ChatEmbed';


export const Stream = ({ params: { channel, service }, chatSize, setChatSize, rustlerCount }) =>
  <MainLayout showFooter={false} rustlerCount={rustlerCount}>
    <Resizeable
      className='grow-1'
      onResize={e => {
        const newChatSize = window.innerWidth - e.pageX;
        setChatSize(newChatSize);
      }}
      >
      <div style={{ width: `calc(100% - ${chatSize}px)` }}>
        <StreamEmbed channel={channel} service={service} />
      </div>
      <div style={{ width: chatSize }}>
        <ChatEmbed />
      </div>
    </Resizeable>
  </MainLayout>
  ;

Stream.propTypes = {
  params: PropTypes.shape({
    channel: PropTypes.string.isRequired,
    service: PropTypes.string.isRequired,
  }),

  chatSize: PropTypes.number.isRequired,

  setChatSize: PropTypes.func.isRequired,
  rustlerCount: MainLayout.propTypes.rustlerCount,
};

export default compose(
  connect(
    state => ({
      chatSize: state.ui.chatSize,
      rustlerCount: state.streams[state.stream] ? [state.streams[state.stream].rustlers, state.streams[state.stream].viewers] : null,
    }),
    {
      setStream,
      setChatSize,
    },
  ),
  setPropTypes({
    params: PropTypes.shape({
      channel: PropTypes.string.isRequired,
      service: PropTypes.string.isRequired,
    }),

    chatSize: PropTypes.number.isRequired,
    rustlerCount: MainLayout.propTypes.rustlerCount,

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
)(Stream);
