import React, { PropTypes } from 'react';

import '../css/StreamThumbnail';


const StreamThumbnail = ({ thumbnail, url, name, service, viewers, ...rest }) =>
  <div className='stream-thumbnail thumbnail' {...rest}>
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
          <span>{name}</span>
          <span className='pull-right label label-as-badge label-success'>
            <span>{viewers}</span>
            {'\u00a0'}
            <span className='glyphicon glyphicon-user' />
          </span>
        </div>
        <div>on {service}</div>
      </a>
    </div>
  </div>
  ;

StreamThumbnail.propTypes = {
  thumbnail: PropTypes.string,
  url: PropTypes.string.isRequired,
  name: PropTypes.string.isRequired,
  service: PropTypes.string.isRequired,
  viewers: PropTypes.number.isRequired,
};

export default StreamThumbnail;
