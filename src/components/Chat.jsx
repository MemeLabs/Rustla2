import React from 'react';
import PropTypes from 'prop-types';
import cs from 'classnames';

const Chat = ({ className, src, style, ...rest }) =>
  <div
    {...rest}
    className={cs('fill-percentage', className)}
    style={{
      position: 'absolute',
      ...style,
    }}
    >
    <div>
      <a href={src} target='_blank' rel='noopener noreferrer'>
        <span className='glyphicon glyphicon-share-alt pull-right' />
      </a>
    </div>
    <iframe
      style={{
        height: 'calc(100% - 1em)',
      }}
      width='100%'
      height='100%'
      marginHeight='0'
      marginWidth='0'
      frameBorder='0'
      scrolling='no'
      src={src}
      />
  </div>
  ;

Chat.propTypes = {
  className: PropTypes.string,
  src: PropTypes.string.isRequired,
  style: PropTypes.object,
};

export default Chat;
