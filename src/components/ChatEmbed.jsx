import React, { PropTypes } from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import get from 'lodash/get';
import cs from 'classnames';


const supportedChats = {
  'azubu': channel => `https://www.azubu.tv/${channel}/chatpopup`,
  'hitbox': channel => `https://www.hitbox.tv/embedchat/${channel}`,
  'twitch-vod': channel => `https://www.twitch.tv/${channel}/chat?popout=`,
  'twitch': channel => `https://www.twitch.tv/${channel}/chat?popout=`,
  'ustream': channel => `https://www.ustream.tv/socialstream/${channel}`,
  'vaughn': channel => `https://vaughnlive.tv/popout/chat/${channel}`,
};

export const supportedChatServices = new Set(Object.keys(supportedChats));

const Chat = ({ src, ...rest }) =>
  <div
    {...rest}
    className={cs('fill-percentage', rest.className)}
    style={{
      position: 'absolute',
      ...rest.style,
    }}
    >
    <div>
      <a href={src} target='_blank' rel='noopener noreferrer'>
        <span className='glyphicon glyphicon-share-alt pull-right' />
      </a>
    </div>
    <iframe
      style={{
        height: 'calc(100% - 1em)',
      }}
      width='100%'
      height='100%'
      marginHeight='0'
      marginWidth='0'
      frameBorder='0'
      scrolling='no'
      src={src}
      />
  </div>
  ;

const ChatEmbed = ({ channel, service, isOtherChatActive }) => {
  let src;
  if (channel && service && typeof supportedChats[service] === 'function') {
    src = supportedChats[service](channel) || src;
  }
  return (
    <div className='fill-percentage' style={{ position: 'relative' }}>
      <Chat style={{ visibility: isOtherChatActive ? 'hidden' : undefined }} src='https://destiny.gg/embed/chat' />
      {
        src ?
        <Chat style={{ visibility: isOtherChatActive ? undefined : 'hidden' }} src={src} />
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
