// @flow

declare var DISCORD_URL: string;

import React from 'react';
import Button from 'react-bootstrap/Button';
import ButtonGroup from 'react-bootstrap/ButtonGroup';
import Nav from 'react-bootstrap/Nav';
import Navbar from 'react-bootstrap/Navbar';
import { Link } from 'react-router-dom';
import { compose } from 'redux';
import { connect } from 'react-redux';
import cs from 'classnames';
import idx from 'idx';
import type { BrowserHistory } from 'history';

import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faDiscord } from '@fortawesome/free-brands-svg-icons';
import { faComments } from '@fortawesome/free-solid-svg-icons';

import '../css/Header';

import type { State } from '../redux/types';
import { toggleChat, showChat, CHAT_HOST_SERVICE, CHAT_HOST_STRIMS, CHAT_HOST_DGG, showHeader } from '../actions';
import { supportedChatServices } from '../util/supported-chats';
import isVod from '../util/is-vod';
import HeaderForm from './HeaderForm';
import NavButtonLink from './NavButtonLink';

type OwnProps = {||};
type Props = {|
  ...OwnProps,
  +chatClosed?: boolean,
  +headerClosed?: boolean,
  +currentStreamService?: string | null,
  +history: BrowserHistory,
  +isDggChat: boolean,
  +isLoggedIn: boolean,
  +isServiceChat: boolean,
  +isStrimsChat: boolean,
  +rustlerCount: number[] | null,
  +showChat: () => {},
  +showHeader: (host: mixed) => {},
  +showDggChat: boolean,
  +toggleChat: (host: mixed) => {}
|};

const Header = ({
  chatClosed,
  headerClosed,
  currentStreamService,
  history,
  classes,
  isDggChat,
  isLoggedIn,
  isServiceChat,
  isStrimsChat,
  rustlerCount,
  showChat,
  showHeader,
  showDggChat,
  toggleChat
}: Props) => {
  let rustlers = null;
  let viewers = null;
  const viewerTitle = currentStreamService && isVod(currentStreamService)
    ? 'Views'
    : 'Viewers';
  if (rustlerCount) {
    const [ rCount, vCount ] = rustlerCount;
    if (rCount) {
      rustlers = (
        <li className="nav-item">
          <a className="nav-link">{rCount.toLocaleString()} Rustlers</a>
        </li>
      );
    }
    if (vCount) {
      viewers = (
        <li className="nav-item">
          <a className="nav-link">
            {vCount.toLocaleString()} {viewerTitle}
          </a>
        </li>
      );
    }
  }

  const chatButtons = [];
  if (currentStreamService) {
    chatButtons.push(<li key="strims" onClick={() => toggleChat(CHAT_HOST_STRIMS)} className={cs('nav-item', { active: isStrimsChat })}><NavButtonLink>Strims Chat</NavButtonLink></li>);

    if (currentStreamService == 'youtube' || currentStreamService == 'twitch' || showDggChat) {
      chatButtons.push(<li key="dgg" onClick={() => toggleChat(CHAT_HOST_DGG)} className={cs('nav-item', { active: isDggChat })}><NavButtonLink>Destiny Chat</NavButtonLink></li>);
    }

    if (supportedChatServices.has(currentStreamService)) {
      chatButtons.push(<li key="service" onClick={() => toggleChat(CHAT_HOST_SERVICE)} className={cs('nav-item', 'text-capitalize', { 'active': isServiceChat })}><NavButtonLink>{currentStreamService} Chat</NavButtonLink></li>);
    }
  }

  let openChat = !chatClosed ? null : (
    <div title="Open Chat" className='open-chat-btn' onClick={showChat}>
        <FontAwesomeIcon icon={faComments} />
    </div>
  );

  let hideHeader = !headerClosed ? (
    <div title="Hide Header" className='close-header-btn' >
      <span className='close-caret' onClick={()=> showHeader(false)} style={{ cursor:'pointer'}}>&#8250;</span>
    </div>
  ) : null ;

  return (
    <>
    <Navbar expand='lg' variant='dark' className={classes}>
      {openChat}
      <Navbar.Brand>
        <Link className='navbar-brand' to='/'>Strims</Link>
      </Navbar.Brand>
      <Nav className='flex-row mr-auto'>{rustlers} {viewers}</Nav>
      <Navbar.Toggle aria-controls="responsive-navbar-nav" />
      <Navbar.Collapse>
        <Nav className='mr-auto'>
        </Nav>
        <Nav>
          {chatButtons}
        </Nav>
        <div className='d-none d-lg-flex'>
          <HeaderForm history={history} />
          <ButtonGroup>
          <Button title="Discord" href={`${DISCORD_URL}`} target="_blank" rel="noopener noreferrer" variant='outline-primary'>
            <FontAwesomeIcon icon={faDiscord} />
          </Button>
            {
              isLoggedIn ?
              <Link className='btn btn-default navbar-btn' to='/profile' title='Profile'>
                <span className='glyphicon glyphicon-user' />
              </Link>
              : null
            }
            {
              isLoggedIn ?
              <Link className='btn btn-default navbar-btn' to='/logout' title='Log Out'>
                <span className='glyphicon glyphicon-log-out' />
              </Link>
              :
              <Button href='/login' variant='outline-primary'>
                <span className='glyphicon glyphicon-log-in' />
              </Button>
            }
          </ButtonGroup>
        </div>
        <Nav className='d-lg-none'>
          {
            isLoggedIn
              ? <Nav.Item as='li'>
                  <Link to='/profile' title='Profile' className="nav-link p-3 pointer">
                    Profile
                  </Link>
                </Nav.Item>
              : null
          }
          {
            isLoggedIn
              ? <Nav.Item as='li'>
                  <Link to='/logout' title='Log Out' className="nav-link p-3 pointer">
                    Log Out
                  </Link>
                </Nav.Item>
              : <Nav.Item as='li'>
                  <Nav.Link href='/login' className="p-3 pointer">
                    Log In
                  </Nav.Link>
                </Nav.Item>
          }
          <Nav.Item as='li'>
            <a href={`${DISCORD_URL}`} target="_blank" rel="noopener noreferrer" title='Discord' className="nav-link p-3 pointer">
              Discord
            </a>
          </Nav.Item>
        </Nav>
      </Navbar.Collapse>
    </Navbar>
    </>
  );
};

function mapStateToProps(state: State): $Shape<Props> {
  return {
    isLoggedIn: state.self.isLoggedIn,
    isDggChat: state.ui.chatHost === CHAT_HOST_DGG,
    isStrimsChat: state.ui.chatHost === CHAT_HOST_STRIMS,
    isServiceChat: state.ui.chatHost === CHAT_HOST_SERVICE,
    currentStreamService: idx(state, _ => _.streams[state.stream].service),
    rustlerCount: state.streams[state.stream] ? [state.streams[state.stream].rustlers, state.streams[state.stream].viewers] : null,
    showDggChat: Boolean(idx(state, _ => _.self.profile.data.show_dgg_chat)),
    chatClosed: !state.ui.showChat,
    headerClosed: !state.ui.showHeader,
  };
}

export default compose(
  connect<Props, OwnProps, _, _, _, _>(
    mapStateToProps,
    {
      toggleChat,
      showChat,
      showHeader
    }
  )
)(Header);
