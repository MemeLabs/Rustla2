import React, { PropTypes } from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { setStream, setChatSize } from '../actions';

import MainLayout from './MainLayout';
import Resizeable from './Resizeable';
import StreamEmbed from './StreamEmbed';


export const Stream = ({ params: { channel, service }, chatSize, setChatSize, rustlerCount }) =>
  <MainLayout showFooter={false} rustlerCount={rustlerCount}>
    <Resizeable
      className='grow-1'
      onResize={e => {
        const newChatSize = window.innerWidth - e.screenX;
        setChatSize(newChatSize);
      }}
      >
      <div style={{ width: `calc(100% - ${chatSize}px)` }}>
        <StreamEmbed channel={channel} service={service} />
      </div>
      <div style={{ width: chatSize }}>
        <iframe
          src='https://destiny.gg/embed/chat'
          frameBorder='0'
          width='100%'
          height='100%'
          />
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

  // Should match `MainLayout.propTypes.rustleCount`. Can't access
  // `MainLayout.propTypes` here, however, because `MainLayout` has been wrapped
  // by recompose. TODO: fix this if possible.
  rustlerCount: PropTypes.arrayOf(PropTypes.number),
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
  lifecycle({
    componentDidMount() {
      const { channel, service } = this.props.params;
      this.props.setStream(channel, service);
    },
  }),
)(Stream);
