import React from 'react';
import PropTypes from 'prop-types';
import { Redirect } from 'react-router-dom';

import AdvancedStreamEmbed from './AdvancedStreamEmbed';
import M3u8StreamEmbed from './M3u8StreamEmbed';
import ThirdPartyWarning from './ThirdPartyWarning';

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
    case 'smashcast':
      return `https://www.smashcast.tv/embed/${channel}?popout=true&autoplay=true`;
    case 'twitch-vod':
      return `https://player.twitch.tv/?video=v${channel}`;
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

const StreamEmbed = ({ channel, service }) => {
  if (service === 'advanced') {
    if (isValidAdvancedUrl(channel)) {
      return (
        <ThirdPartyWarning channel={channel}>
          <AdvancedStreamEmbed channel={channel} />
        </ThirdPartyWarning>
      );
    }
    return <Redirect to='/' />;
  }

  if (service === 'm3u8') {
    return (
      <ThirdPartyWarning channel={channel}>
        <M3u8StreamEmbed src={channel} />
      </ThirdPartyWarning>
    );
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
        />
    );
  }
  return <div className='jiggle-background' style={{ width: '100%', height: '100%' }} />;
};

StreamEmbed.propTypes = {
  channel: PropTypes.string,
  service: PropTypes.oneOf([
    'advanced',
    'afreeca',
    'angelthump',
    'dailymotion',
    'facebook',
    'm3u8',
    'nsfw-chaturbate',
    'smashcast',
    'twitch-vod',
    'twitch',
    'ustream',
    'vaughn',
    'youtube-playlist',
    'youtube',
  ]),
};

export default StreamEmbed;
