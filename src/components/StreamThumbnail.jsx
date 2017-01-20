import React, { PropTypes } from 'react';
import { Link } from 'react-router';

import '../css/StreamThumbnail';


const StreamThumbnail = ({ overrustle, channel, service, thumbnail, live, rustlers, ...rest }) => {
  const url = overrustle ? overrustle : `${service}/${channel}`;
  return (
    <div className='stream-thumbnail'>
      <Link to={url}>
        <img
          className={thumbnail ? null : 'jiggle'}
          src={thumbnail ? thumbnail : '/image/jigglymonkey.png'}
          />
      </Link>
      <div className='stream-caption'>
        <Link to={url}>
          <div>
            <span>{channel}</span>
            <span className='pull-right label label-as-badge label-success'>
              <span>{rustlers}</span>
              {'\u00a0'}
              <span className='glyphicon glyphicon-user' />
            </span>
          </div>
          <div>on {service}</div>
        </Link>
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
  twitch_channel_id: PropTypes.number,
};

export default StreamThumbnail;
