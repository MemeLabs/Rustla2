/* global THUMBNAIL_REFRESH_INTERVAL */

import React from 'react';
import PropTypes from 'prop-types';
import { Link } from 'react-router-dom';

import '../css/StreamThumbnail';

const StreamThumbnail = ({ overrustle_id, channel, service, title, thumbnail, live, rustlers, ...rest }) => {
  const url = overrustle_id ? overrustle_id : `${service}/${channel}`;

  let text = overrustle_id && overrustle_id !== channel
    ? `${overrustle_id} via ${channel}`
    : `${channel}`;
  if (title) {
    text = overrustle_id ? `${title} presented by ${overrustle_id}` : title;
  }

  let thumbnailProps = { className: 'thumbnail-image thumbnail-default-image' };
  if (thumbnail) {
    const epochMinute = Math.floor(Date.now() / (THUMBNAIL_REFRESH_INTERVAL || 60000));
    const thumbnailUrl = live ? `${thumbnail}?${epochMinute}` : thumbnail;

    thumbnailProps = {
      className: 'thumbnail-image',
      style: { backgroundImage: `url(${thumbnailUrl})` },
    };
  }

  return (
    <div className={`stream-thumbnail stream-thumbnail-${service}`} title={text} {...rest}>
      <Link to={url} {...thumbnailProps} />
      <div className='stream-caption'>
        <Link to={url}>
          <span className={`pull-right label label-as-badge label-${live ? 'success' : 'danger'}`}>
            <span>{rustlers}</span>
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
};

export default StreamThumbnail;
