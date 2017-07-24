import React from 'react';
import cs from 'classnames';

const Chat = ({ src, ...rest }) =>
  <div
    {...rest}
    className={cs('fill-percentage', rest.className)}
    style={{
      position: 'absolute',
      ...rest.style,
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

export default Chat;
