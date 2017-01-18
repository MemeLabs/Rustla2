import React, { PropTypes } from 'react';

import '../css/StreamThumbnail';


const StreamThumbnail = ({ overrustle, channel, service, thumbnail, live, rustlers, viewers, ...rest }) => {
  const url = overrustle ? overrustle : `${service}/${channel}`;
  return (
    <div className='stream-thumbnail' {...rest}>
      {
        thumbnail ?
        <a href={url}>
          <img src={thumbnail} />
        </a>
        : null
      }
      <div className='stream-caption'>
        <a
          href={url}
          >
          <div>
            <span>{channel}</span>
            <span className='pull-right label label-as-badge label-success'>
              <span>{rustlers}</span>
              {'\u00a0'}
              <span className='glyphicon glyphicon-user' />
            </span>
          </div>
          <div>on {service}</div>
        </a>
      </div>
    </div>
  );
};

StreamThumbnail.propTypes = {
  overrustle: PropTypes.string,
  channel: PropTypes.string.isRequired,
  service: PropTypes.string.isRequired,
  thumbnail: PropTypes.string,
  live: PropTypes.bool.isRequired,
  viewers: PropTypes.number,
  rustlers: PropTypes.number.isRequired,
};

export default StreamThumbnail;
