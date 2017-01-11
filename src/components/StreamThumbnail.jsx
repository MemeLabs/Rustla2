import React, { PropTypes } from 'react';


const StreamThumbnail = ({ thumbnai, url, name, service, viewers, ...rest }) =>
  <div
    style={{
      padding: 4,
      marginBottom: 20,
      borderRadius: 4,
    }}
    {...rest}
    >
    {
      thumbnail ?
      <a href={url}>
        <img
          style={{
            maxWidth: '100%',
            height: 'auto',
          }}
          src={thumbnail}
          />
      </a>
      : null
    }
    <div
      style={{
        padding: 9,
        color: '#333',
      }}
      >
      <a
        href={url}
        style={{
          display: 'block',
          whiteSpace: 'nowrap',
          textOveflow: 'ellipsis',
          overflow: 'hidden',
        }}
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
