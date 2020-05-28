// @flow

import React from 'react';
import {useClickAway} from 'react-use';

import { connect } from 'react-redux';
import { modifyStream, banViewers } from '../actions';

import '../css/StreamAdminMenu';

type OwnProps = {
  id: number;
  channel: string;
  service: string;
  overrustle_id: string,
  afk: boolean;
  nsfw: boolean;
  promoted: boolean;
  hidden: boolean;
};

type DispatchProps = {
  modifyStream: typeof modifyStream,
  banViewers: typeof banViewers;
};

type Props = OwnProps & DispatchProps;

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
}: Props) => {
  const containerRef = React.useRef();
  const [isOpen, setIsOpen] = React.useState(false);

  const toggleIsOpen = () => setIsOpen(prev => !prev);
  useClickAway(containerRef, () => setIsOpen(false));

  const handleToggleClick = (prop: string, state: boolean) => {
    modifyStream({channel, service, overrustle_id}, {[prop]: state});
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

export default connect<Props, OwnProps, any, DispatchProps, any, any>(
  null,
  {modifyStream, banViewers},
)(StreamAdminMenu);
