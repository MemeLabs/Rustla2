import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import get from 'lodash/get';

import { toggleChat } from '../actions';
import supportedChats, { supportedChatServices } from '../util/supported-chats';
import Chat from './Chat';
import LazyLoadOnce from './LazyLoadOnce';


const ChatEmbed = ({ channel, onClose, service, isOtherChatActive }) => {
  let src;
  if (channel && service && typeof supportedChats[service] === 'function') {
    src = supportedChats[service](channel) || src;
  }

  return (
    <div className='fill-percentage' style={{ position: 'relative' }}>
      <Chat
        onClose={onClose}
        style={{ visibility: isOtherChatActive ? 'hidden' : undefined }}
        src='https://destiny.gg/embed/chat'
        />
      {
        src ?
          <LazyLoadOnce visible={isOtherChatActive}>
            <Chat
              onClose={onClose}
              style={{ visibility: isOtherChatActive ? undefined : 'hidden' }}
              src={src}
              />
          </LazyLoadOnce>
        : null
      }
    </div>
  );
};

ChatEmbed.propTypes = {
  channel: PropTypes.string,
  onClose: PropTypes.func,
  service: PropTypes.string,
};

export default compose(
  connect(
    state => {
      return {
        isOtherChatActive: state.ui.isOtherChatActive,
        channel: get(state, ['streams', state.stream, 'channel']),
        service: get(state, ['streams', state.stream, 'service']),
      };
    },
    {
      toggleChat,
    },
  ),
  lifecycle({
    componentWillMount() {
      if (this.props.isOtherChatActive && !supportedChatServices.has(this.props.service)) {
        this.props.toggleChat(false);
      }
    },
  }),
)(ChatEmbed);
