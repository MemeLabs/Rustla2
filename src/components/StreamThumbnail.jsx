/* global THUMBNAIL_REFRESH_INTERVAL */

import React from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import { Link } from 'react-router-dom';
import idx from 'idx';
import lifecycle from 'recompose/lifecycle';
import PropTypes from 'prop-types';

import { fetchProfileIfLoggedIn } from '../actions';
import { generateColor } from '../util/color';
import StreamAdminMenu from './StreamAdminMenu';

import '../css/StreamThumbnail';

const getStreamTitle = (overrustle_id, channel, title, service) => {
  switch (service) {
    case 'angelthump': {
      const presenter = overrustle_id && overrustle_id !== channel
        ? `${overrustle_id} via ${channel}`
        : channel;
      const content = title
        ? `presents ${title}`
        : 'on angelthump';
      return `${presenter} ${content}`;
    }
    case 'twitch-vod':
      return title ? `${channel}: ${title}` : `${channel} on twitch`;
    case 'twitch':
      return title ? `${channel} playing ${title}` : `${channel} on twitch`;
    default:
      return title ? title : `${channel} on ${service}`;
  }
};

const getStreamServiceLink = (channel, service) => {
  switch (service) {
    case "angelthump": 
      return "https://strims.gg/angelthump/" + channel;
    case "twitch-vod":
      return "https://www.twitch.tv/videos/" + channel;
    case "twitch":
      return "https://www.twitch.tv/" + channel;
    case "youtube":
      return "https://www.youtube.com/watch?v=" + channel;
    default:
      return "https://www." + service + ".com/" + channel;
  }
};

const StreamThumbnail = ({
  id,
  afk_rustlers = 0,
  channel,
  live = false,
  afk = false,
  nsfw = false,
  promoted = false,
  hidden = false,
  overrustle_id,
  rustlers,
  service,
  thumbnail,
  title,
  isAdmin,
}) => {
  const color = generateColor(channel + service);
  const url = overrustle_id ? overrustle_id : `${service}/${channel}`;
  const text = getStreamTitle(overrustle_id, channel, title, service);

  let thumbnailProps = { className: 'thumbnail-image thumbnail-default-image' };
  if (thumbnail) {
    const epochMinute = Math.floor(Date.now() / (THUMBNAIL_REFRESH_INTERVAL || 60000));
    const thumbnailUrl = live ? `${thumbnail}?${epochMinute}` : thumbnail;

    thumbnailProps = {
      className: `thumbnail-image ${nsfw ? 'thumbnail-image-nsfw' : ''}`,
      style: { backgroundImage: `url(${thumbnailUrl})` },
    };
  }

  const adminMenu = isAdmin && (
    <StreamAdminMenu
      id={id}
      channel={channel}
      service={service}
      overrustle_id={overrustle_id}
      afk={afk}
      nsfw={nsfw}
      promoted={promoted}
      hidden={hidden}
    />
  );

  return (
    <div style={{position:'relative'}}>
      {adminMenu}
      <a
        className={`stream-thumbnail stream-thumbnail-transparent`}
        title={text}
        target="_blank"
        rel="noopener noreferrer"
        href={getStreamServiceLink(channel, service) }/>
      <Link
        className={`stream-thumbnail stream-thumbnail-${service}`}
        title={text}
        to={url}>
        <div {...thumbnailProps} />
        <div className='stream-caption'>
          <span className='thumbnail-text' style={{borderLeftColor: color}}>
            {text}
          </span>
          <span
            title={afk_rustlers ? `${afk_rustlers} afk` : ''}
            className={`badge badge-${live ? 'success' : 'danger'}`}>
            {rustlers.toLocaleString()}
            <span className='glyphicon glyphicon-user' />
          </span>
        </div>
      </Link>
    </div>
  );
};

StreamThumbnail.propTypes = {
  id: PropTypes.number,
  afk_rustlers: PropTypes.number,
  channel: PropTypes.string.isRequired,
  live: PropTypes.bool,
  afk: PropTypes.bool,
  nsfw: PropTypes.bool,
  promoted: PropTypes.bool,
  hidden: PropTypes.bool,
  overrustle_id: PropTypes.string,
  rustlers: PropTypes.number.isRequired,
  service: PropTypes.string.isRequired,
  thumbnail: PropTypes.string,
  title: PropTypes.string,
  isAdmin: PropTypes.bool,
};

export default compose(
  connect(
    (state, ownProps) => ({
      isAdmin: idx(state, _ => _.self.profile.data.is_admin),
    }),
    {
      fetchProfileIfLoggedIn,
    },
  ),
  lifecycle({
    componentDidMount() {
      this.props.fetchProfileIfLoggedIn();
    },
  }),
)(StreamThumbnail);
