import React from 'react';
import PropTypes from 'prop-types';
import cs from 'classnames';

/* eslint-disable no-empty-function */
function noop() {}
/* eslint-enable no-empty-function */

const Chat = ({ className, onClose = noop, src, style, ...rest }) =>
  <div
    {...rest}
    className={cs('fill-percentage', className)}
    style={{
      position: 'absolute',
      ...style,
    }}
    >
    <div>
      <a title='Close' onClick={onClose}>
        <span className='glyphicon glyphicon-remove pull-right' />
      </a>
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
  onClose: PropTypes.func,
  src: PropTypes.string.isRequired,
  style: PropTypes.object,
};

export default Chat;
