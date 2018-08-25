/* global THUMBNAIL_REFRESH_INTERVAL */

import React from 'react';
import PropTypes from 'prop-types';
import { Link } from 'react-router-dom';

import '../css/StreamThumbnail';

const getStreamTitle = ({ overrustle_id, channel, title, service }) => {
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

const StreamThumbnail = (props) => {
  const { overrustle_id, channel, service, nsfw, thumbnail, live, rustlers, afk_rustlers, ...rest } = props;
  const url = overrustle_id ? overrustle_id : `${service}/${channel}`;
  const text = getStreamTitle(props);

  let thumbnailProps = { className: 'thumbnail-image thumbnail-default-image' };
  if (thumbnail) {
    const epochMinute = Math.floor(Date.now() / (THUMBNAIL_REFRESH_INTERVAL || 60000));
    const thumbnailUrl = live ? `${thumbnail}?${epochMinute}` : thumbnail;

    thumbnailProps = {
      className: `thumbnail-image ${nsfw ? 'thumbnail-image-nsfw' : ''}`,
      style: { backgroundImage: `url(${thumbnailUrl})` },
    };
  }

  return (
    <div className={`stream-thumbnail stream-thumbnail-${service}`} title={text} {...rest}>
      <Link to={url} {...thumbnailProps} />
      <div className='stream-caption'>
        <Link to={url}>
          <span className={`pull-right label label-as-badge label-${live ? 'success' : 'danger'}`}>
            <span title={`${afk_rustlers} afk`}>{rustlers}</span>
            {'\u00a0'}
            <span className='glyphicon glyphicon-user' />
          </span>
          <span className='thumbnail-text'>{text}</span>
        </Link>
      </div>
    </div>
  );
};

StreamThumbnail.propTypes = {
  overrustle_id: PropTypes.string,
  channel: PropTypes.string.isRequired,
  service: PropTypes.string.isRequired,
  title: PropTypes.string,
  thumbnail: PropTypes.string,
  live: PropTypes.bool.isRequired,
  rustlers: PropTypes.number.isRequired,
  nsfw: PropTypes.bool,
};

export default StreamThumbnail;
