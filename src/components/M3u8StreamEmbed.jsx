import React, { Component } from 'react';
import PropTypes from 'prop-types';
import clappr from 'clappr';

class M3u8StreamEmbed extends Component {
  static propTypes = {
    src: PropTypes.string.isRequired,
  };

  createPlayer() {
    if (this.player) {
      this.destroyPlayer();
    }

    this.player = new clappr.Player({
      parent: this.playerNode,
      source: this.props.src,
      width: '100%',
      height: '100%',
    });
  }

  destroyPlayer() {
    if (this.player) {
      this.player.destroy();
    }

    this.player = null;
  }

  componentDidMount() {
    this.createPlayer();
  }

  componentWillUnmount() {
    this.destroyPlayer();
  }

  shouldComponentUpdate(nextProps, nextState) {
    if (nextProps.src !== this.props.src) {
      this.createPlayer();
    }
    return false;
  }

  render() {
    return <div className='fill-percentage' ref={c => this.playerNode = c} />;
  }
}

export default M3u8StreamEmbed;
