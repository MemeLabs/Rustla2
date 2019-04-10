/* global CHAT2_DOMAIN, CHAT2_URL, CHAT_URL */

import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import idx from 'idx';

import { toggleChat, CHAT_HOST_SERVICE, CHAT_HOST_STRIMS, CHAT_HOST_DGG } from '../actions';
import supportedChats, { supportedChatServices } from '../util/supported-chats';
import Chat from './Chat';
import LazyLoadOnce from './LazyLoadOnce';


const ChatEmbed = ({
  channel,
  onClose,
  service,
  isDggChat,
  isStrimsChat,
  isServiceChat,
}) => {
  let src;
  if (channel && service && typeof supportedChats[service] === 'function') {
    src = supportedChats[service](channel) || src;
  }

  return (
    <div className='fill-percentage' style={{ position: 'relative' }}>
      <Chat
        onClose={onClose}
        style={{ visibility: isStrimsChat ? undefined : 'hidden' }}
        src={window.location.toString() === CHAT2_DOMAIN ? CHAT2_URL : CHAT_URL}
        />
      <LazyLoadOnce visible={isDggChat}>
        <Chat
          onClose={onClose}
          style={{ visibility: isDggChat ? undefined : 'hidden' }}
          src='https://destiny.gg/embed/chat'
          />
      </LazyLoadOnce>
      {
        src ?
          <LazyLoadOnce visible={isServiceChat}>
            <Chat
              onClose={onClose}
              style={{ visibility: isServiceChat ? undefined : 'hidden' }}
              src={src}
              />
          </LazyLoadOnce>
        : null
      }
    </div>
  );
};

ChatEmbed.propTypes = {
  isDggChat: PropTypes.bool,
  isStrimsChat: PropTypes.bool,
  isServiceChat: PropTypes.bool,
  channel: PropTypes.string,
  onClose: PropTypes.func,
  service: PropTypes.string,
};

export default compose(
  connect(
    state => ({
      isDggChat: state.ui.chatHost === CHAT_HOST_DGG,
      isStrimsChat: state.ui.chatHost === CHAT_HOST_STRIMS,
      isServiceChat: state.ui.chatHost === CHAT_HOST_SERVICE,
      channel: idx(state, _ => _.streams[state.stream].channel),
      service: idx(state, _ => _.streams[state.stream].service),
    }),
    {
      toggleChat,
    },
  ),
  lifecycle({
    componentWillMount() {
      if (this.props.isServiceChat && !supportedChatServices.has(this.props.service)) {
        this.props.toggleChat(false);
      }
    },
  }),
)(ChatEmbed);
