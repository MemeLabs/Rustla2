// @flow

import React from 'react';
import PropTypes from 'prop-types';
import {useClickAway} from 'react-use';

import { connect } from 'react-redux';
import { modifyStream, banViewers } from '../actions';

// import '../css/StreamThumbnail';
// import idx from 'idx';
// import { compose } from 'redux';
// import { connect } from 'react-redux';
// import { fetchProfile } from '../actions';
// import lifecycle from 'recompose/lifecycle';

import '../css/StreamAdminMenu';


type StreamAdminMenuProps = {
  id: number;
  channel: string;
  service: string;
  overrustle_id: string,
  afk: boolean;
  nsfw: boolean;
  promoted: boolean;
  hidden: boolean;
  modifyStream: any;
  banViewers: any;
};

const StreamAdminMenu = ({
  id,
  channel,
  service,
  overrustle_id,
  afk,
  nsfw,
  promoted,
  hidden,
  modifyStream,
  banViewers,
}: StreamAdminMenuProps) => {
  const containerRef = React.useRef();
  const [isOpen, setIsOpen] = React.useState(false);

  const toggleIsOpen = () => setIsOpen(prev => !prev);
  useClickAway(containerRef, () => setIsOpen(false));

  const handleToggleClick = (prop: string, state: boolean) => {
    modifyStream({channel, service, overrustle_id}, {[prop]: state});
    console.log(prop, state);
  };

  const handleBanClick = () => banViewers(id);

  const menu = isOpen && (
    <div className="stream-admin-menu">
      <button onClick={() => handleToggleClick("afk", !afk)}>
        toggle afk {afk ? "off" : "on"}
      </button>
      <button onClick={() => handleToggleClick("nsfw", !nsfw)}>
        toggle nsfw {nsfw ? "off" : "on"}
      </button>
      <button onClick={() => handleToggleClick("promoted", !promoted)}>
        toggle promoted {promoted ? "off" : "on"}
      </button>
      <button onClick={() => handleToggleClick("hidden", !hidden)}>
        toggle hidden {hidden ? "off" : "on"}
      </button>
      <button onClick={() => handleToggleClick("removed", true)}>remove</button>
      <button onClick={handleBanClick}>ban viewers</button>
    </div>
  );

  return (
    <div className="stream-admin-menu-container" ref={containerRef}>
      <button onClick={toggleIsOpen}>&bull;&bull;&bull;</button>
      {menu}
    </div>
  );
};

export default connect(
  null,
  (dispatch) => ({
    modifyStream: (stream, updates) => dispatch(modifyStream(stream, updates)),
    banViewers: (streamId) => dispatch(banViewers(streamId)),
  }),
)(StreamAdminMenu);
