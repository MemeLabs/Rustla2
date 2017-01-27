import React, { PropTypes } from 'react';
import Draggable from 'react-draggable';
import cs from 'classnames';

import '../css/Resizeable';

// TODO - connect these handlers to action dispatchers
const logfn = (...args) => () => console.log(...args);

const Resizeable = ({ children, barSize = 5, axis = 'x', onStart = logfn('onstart'), onStop = logfn('onstop'), ...rest }) => {
  if (children.length !== 2) {
    throw new Error('Resizeable: unsupported children length');
  }
  const content = [];
  children.forEach((child, i) => {
    content.push(child);
    if (i % 2 === 0) {
      content.push(
        <Draggable
          axis={axis}
          onStart={onStart}
          onStop={onStop}
          bounds='parent'
          >
          <div className={`resizeable-bar resizeable-bar-${axis}`}>
            <div className='resizeable-bar-child' style={{ [axis === 'x' ? 'width' : 'height']: barSize }} />
          </div>
        </Draggable>
      );
    }
  });
  return React.createElement('div', {
    ...rest,
    className: cs('resizeable', `resizeable-${axis}`, rest.className),
  }, ...content);
};

Resizeable.propTypes = {
  children: PropTypes.arrayOf(PropTypes.node).isRequired,
  barSize: PropTypes.number,
  axis: PropTypes.oneOf(['x', 'y']),
  onStart: PropTypes.func,
  onStop: PropTypes.func,
};

export default Resizeable;
