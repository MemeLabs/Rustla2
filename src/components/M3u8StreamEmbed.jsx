// @flow

import React, { Component } from 'react';
import clappr from 'clappr';

type Props = {
  src: string
};

class M3u8StreamEmbed extends Component<Props> {
  player: clappr.Player;
  playerNode: HTMLDivElement | null;

  createPlayer() {
    if (this.player) {
      this.destroyPlayer();
    }

    this.player = new clappr.Player({
      parent: this.playerNode,
      source: this.props.src,
      width: '100%',
      height: '100%',
      autoPlay: true,
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

  shouldComponentUpdate(nextProps: Props) {
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
