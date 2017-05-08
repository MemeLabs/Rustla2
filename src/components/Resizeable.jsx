import React from 'react';
import PropTypes from 'prop-types';
import Draggable from 'react-draggable';
import cs from 'classnames';

import '../css/Resizeable';


class Resizeable extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      dragging: false,
    };
    this.dragStart = this.dragStart.bind(this);
    this.dragStop = this.dragStop.bind(this);
  }

  static propTypes = {
    children: PropTypes.arrayOf(PropTypes.node).isRequired,
    barSize: PropTypes.number,
    axis: PropTypes.oneOf(['x', 'y']),
    onResize: PropTypes.func,
  };

  dragStart(e, dragData) {
    if (!this.state.dragging) {
      this.setState({ dragging: true });
    }
  }
  dragStop(e, dragData) {
    if (this.state.dragging) {
      this.setState({ dragging: false });
    }
    if (typeof this.props.onResize === 'function') {
      this.props.onResize(e, dragData);
    }
  }

  render() {
    const { children, barSize = 5, axis = 'x', onResize, ...rest } = this.props;
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
            onStart={this.dragStart}
            onStop={this.dragStop}
            bounds='parent'
            position={{ x: 0, y: 0 }}
            >
            <div className={`resizeable-bar resizeable-bar-${axis}`}>
              <div className='resizeable-bar-child' style={{ [axis === 'x' ? 'width' : 'height']: barSize }} />
            </div>
          </Draggable>
        );
      }
    });
    if (this.state.dragging) {
      content.push(<div className='resizeable-backdrop' />);
    }
    return React.createElement('div', {
      ...rest,
      className: cs('resizeable', `resizeable-${axis}`, rest.className),
    }, ...content);
  }
}

export default Resizeable;
