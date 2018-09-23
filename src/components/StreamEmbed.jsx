// @flow

import React from 'react';
import type { StatelessFunctionalComponent } from 'react';
import { Redirect } from 'react-router-dom';
import { withProps } from 'recompose';
import type { HOC } from 'recompose';

import AdvancedStreamEmbed from './AdvancedStreamEmbed';
import AdvancedStreamWarning from './AdvancedStreamWarning';
import M3u8StreamEmbed from './M3u8StreamEmbed';
import NsfwStreamWarning from './NsfwStreamWarning';
import StreamWarning from './StreamWarning';

// Use `window.URL` as our WHATWG `URL` implementation. See
// <http://caniuse.com/#feat=url> for the browsers which do not support this.
const isValidAdvancedUrl = require('../util/is-valid-advanced-url')(window.URL);

const getSrc = (channel: string, service: string) => {
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

type StreamService =
  | 'advanced'
  | 'afreeca'
  | 'angelthump'
  | 'dailymotion'
  | 'facebook'
  | 'm3u8'
  | 'nsfw-chaturbate'
  | 'smashcast'
  | 'twitch-vod'
  | 'twitch'
  | 'ustream'
  | 'vaughn'
  | 'youtube-playlist'
  | 'youtube';

type Props = {
  channel: string,
  service: StreamService,
  nsfw?: boolean
};

const StreamEmbed = ({ channel, service, nsfw = false }: Props) => {
  const withChannelProp = withProps((props) => ({ channel, ...props }));
  const withSrcProp = withProps((props) => ({ src: channel, ...props }));

  if (service === 'advanced') {
    if (isValidAdvancedUrl(channel)) {
      return (
        <StreamWarning
          stream={withChannelProp(AdvancedStreamEmbed)}
          warning={withChannelProp(AdvancedStreamWarning)}
        />
      );
    }
    return <Redirect to='/' />;
  }

  if (service === 'm3u8') {
    return (
      <StreamWarning
        stream={withSrcProp(M3u8StreamEmbed)}
        warning={withChannelProp(AdvancedStreamWarning)}
      />
    );
  }

  const src = getSrc(channel, service);
  if (src) {
    const frame: StatelessFunctionalComponent<{}> = () => <iframe
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
    />;
    if (nsfw) {
      return (
        <StreamWarning
          stream={frame}
          warning={NsfwStreamWarning}
        />
      );
    }

    // $FlowFixMe
    return frame();
  }
  return <div className='jiggle-background' style={{ width: '100%', height: '100%' }} />;
};

export default StreamEmbed;
