import React, { PropTypes } from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import get from 'lodash/get';


const supportedChats = {
  'azubu': channel => `https://www.azubu.tv/${channel}/chatpopup`,
  'hitbox': channel => `https://www.hitbox.tv/embedchat/${channel}`,
  'twitch-vod': channel => `https://www.twitch.tv/${channel}/chat?popout=`,
  'twitch': channel => `https://www.twitch.tv/${channel}/chat?popout=`,
  'ustream': channel => `https://www.ustream.tv/socialstream/${channel}`,
  'vaughn': channel => `https://vaughnlive.tv/popout/chat/${channel}`,
};

export const supportedChatServices = new Set(Object.keys(supportedChats));

const ChatEmbed = ({ channel, service, isOtherChatActive }) => {
  let src;
  if (channel && service && typeof supportedChats[service] === 'function') {
    src = supportedChats[service](channel) || src;
  }
  return (
    <div className='fill-percentage' style={{ position: 'relative' }}>
      <iframe
        width='100%'
        height='100%'
        marginHeight='0'
        marginWidth='0'
        frameBorder='0'
        scrolling='no'
        style={{ position: 'absolute', visibility: isOtherChatActive ? 'hidden' : undefined }}
        src='https://destiny.gg/embed/chat'
        />
      {
        src ?
        <iframe
          width='100%'
          height='100%'
          marginHeight='0'
          marginWidth='0'
          frameBorder='0'
          scrolling='no'
          style={{ position: 'absolute', visibility: isOtherChatActive ? undefined : 'hidden' }}
          src={src}
          />
        : null
      }
    </div>
  );
};

ChatEmbed.propTypes = {
  channel: PropTypes.string,
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
  ),
)(ChatEmbed);
