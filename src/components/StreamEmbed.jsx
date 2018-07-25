import React from 'react';
import PropTypes from 'prop-types';
import { Redirect } from 'react-router-dom';

import AdvancedStreamEmbed from './AdvancedStreamEmbed';

// Use `window.URL` as our WHATWG `URL` implementation. See
// <http://caniuse.com/#feat=url> for the browsers which do not support this.
const isValidAdvancedUrl = require('../util/is-valid-advanced-url')(window.URL);

const getSrc = (channel, service) => {
  switch (service) {
    case 'angelthump':
      return `https://angelthump.com/embed/${channel}`;
    case 'dailymotion':
      return `//www.dailymotion.com/embed/video/${channel}?autoplay=1`;
    case 'facebook':
      return `https://www.facebook.com/video/embed?video_id=${channel}&autoplay=true`;
    case 'nsfw-chaturbate':
      return `https://chaturbate.com/embed/${channel}?bgcolor=black`;
    case 'twitch-vod':
      return `https://player.twitch.tv/?volume=0.71&video=v${channel}`;
    case 'twitch':
      return `https://player.twitch.tv/?channel=${channel}`;
    case 'ustream':
      return `https://www.ustream.tv/embed/${channel}?autoplay=true&html5ui=true`;
    case 'vaughn':
      return `https://vaughnlive.tv/embed/video/${channel}`;
    case 'youtube-playlist':
      return `https://www.youtube.com/embed/videoseries?list=${channel}&autoplay=1`;
    case 'youtube':
      return `https://www.youtube.com/embed/${channel}?autoplay=1`;
    case 'afreeca':
      return `https://play.afreecatv.com/${channel}/embed`;
  }
};

const StreamEmbed = ({ channel, service, ...rest }) => {
  if (service === 'advanced') {
    if (isValidAdvancedUrl(channel)) {
      return <AdvancedStreamEmbed channel={channel} />;
    }
    return <Redirect to='/' />;
  }

  const src = getSrc(channel, service);
  if (src) {
    return (
      <iframe
        width='100%'
        height='100%'
        marginHeight='0'
        marginWidth='0'
        frameBorder='0'
        scrolling='no'
        seamless
        allow='autoplay; fullscreen'
        allowFullScreen
        src={src}
        {...rest}
        />
    );
  }
  return <div className='jiggle-background' style={{ width: '100%', height: '100%' }} {...rest} />;
};

StreamEmbed.propTypes = {
  channel: PropTypes.string,
  service: PropTypes.oneOf([
    'advanced',
    'afreeca',
    'angelthump',
    'dailymotion',
    'facebook',
    'nsfw-chaturbate',
    'twitch-vod',
    'twitch',
    'ustream',
    'vaughn',
    'youtube-playlist',
    'youtube',
  ]),
};

export default StreamEmbed;
