import React, { PropTypes } from 'react';
import { Link } from 'react-router';

import '../css/StreamThumbnail';


const StreamThumbnail = ({ overrustle_id, channel, service, thumbnail, live, rustlers, ...rest }) => {
  const url = overrustle_id ? overrustle_id : `${service}/${channel}`;
  const text = overrustle_id ? `${overrustle_id} via ${channel} on ${service}` : `${channel} on ${service}`;
  return (
    <div className='stream-thumbnail' {...rest}>
      <Link to={url}>
        {thumbnail ? <img src={thumbnail} /> : <img className='jiggle-position' src='/image/jigglymonkey.png' />}
      </Link>
      <div className='stream-caption'>
        <Link to={url}>
          <span className={`pull-right label label-as-badge label-${live ? 'success' : 'danger'}`}>
            <span>{rustlers}</span>
            {'\u00a0'}
            <span className='glyphicon glyphicon-user' />
          </span>
          <span>{text}</span>
        </Link>
      </div>
    </div>
  );
};

StreamThumbnail.propTypes = {
  overrustle_id: PropTypes.string,
  channel: PropTypes.string.isRequired,
  service: PropTypes.string.isRequired,
  thumbnail: PropTypes.string,
  live: PropTypes.bool.isRequired,
  rustlers: PropTypes.number.isRequired,
};

export default StreamThumbnail;
