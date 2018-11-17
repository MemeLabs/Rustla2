/* global DONATE_PAYPAL_URL */
import React from 'react';
import PropTypes from 'prop-types';
import { Link } from 'react-router-dom';
import { compose } from 'redux';
import { connect } from 'react-redux';
import cs from 'classnames';
import idx from 'idx';

import '../css/Header';

import { toggleChat, CHAT_HOST_SERVICE, CHAT_HOST_STRIMS, CHAT_HOST_DGG } from '../actions';
import { supportedChatServices } from '../util/supported-chats';
import isVod from '../util/is-vod';
import HeaderForm from './HeaderForm';
import NavButtonLink from './NavButtonLink';


const Header = ({
  rustlerCount,
  isLoggedIn,
  isDggChat,
  isStrimsChat,
  isServiceChat,
  currentStreamService,
  toggleChat,
  history,
}) => {
  let rustlers = null;
  let viewers = null;
  const viewerTitle = isVod(currentStreamService) ? 'Views' : 'Viewers';
  if (rustlerCount) {
    const [ rCount, vCount ] = rustlerCount;
    if (rCount) {
      rustlers = (
        <li className="nav-item">
          <a className="nav-link">{rCount} Rustlers</a>
        </li>
      );
    }
    if (vCount) {
      viewers = (
        <li className="nav-item">
          <a className="nav-link">
            {vCount} {viewerTitle}
          </a>
        </li>
      );
    }
  }
  return (
    <nav
      className='header navbar navbar-dark navbar-expand-lg px-3'
      role='navigation'
    >
      <div className='navbar-header'>
        <Link className='navbar-brand' to='/'>Strims</Link>
      </div>
      <div className='collapse navbar-collapse'>
        <ul className='navbar-nav mr-auto'>
          {rustlers}
          {viewers}
          {DONATE_PAYPAL_URL ? <li className='nav-item'><a className='nav-link' target='_blank' rel='noopener noreferrer' href={DONATE_PAYPAL_URL}><span className='header-donate'>Donate</span></a></li> : null}
        </ul>
        <ul className='navbar-nav'>
          {!currentStreamService ? null : <li onClick={() => toggleChat(CHAT_HOST_STRIMS)} className={cs('nav-item', { active: isStrimsChat })}><NavButtonLink>Strims Chat</NavButtonLink></li>}
          {!currentStreamService ? null : <li onClick={() => toggleChat(CHAT_HOST_DGG)} className={cs('nav-item', { active: isDggChat })}><NavButtonLink>Destiny Chat</NavButtonLink></li>}
          {!currentStreamService || !supportedChatServices.has(currentStreamService) ? null : <li onClick={() => toggleChat(CHAT_HOST_SERVICE)} className={cs('nav-item', 'text-capitalize', { 'active': isServiceChat })}><NavButtonLink>{currentStreamService} Chat</NavButtonLink></li>}
        </ul>
        <HeaderForm history={history} />
        <div className='btn-group'>
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
            <a className='btn btn-default navbar-btn' href='/login' title='Log In'>
              <span className='glyphicon glyphicon-log-in' />
            </a>
          }
        </div>
      </div>
    </nav>
  );
};

Header.propTypes = {
  isLoggedIn: PropTypes.bool.isRequired,
  isDggChat: PropTypes.bool.isRequired,
  isStrimsChat: PropTypes.bool.isRequired,
  isServiceChat: PropTypes.bool.isRequired,
  currentStreamService: PropTypes.string,
  toggleChat: PropTypes.func.isRequired,
  history: PropTypes.object,
  rustlerCount: PropTypes.arrayOf(PropTypes.number)
};

export default compose(
  connect(
    state => ({
      isLoggedIn: state.self.isLoggedIn,
      isDggChat: state.ui.chatHost === CHAT_HOST_DGG,
      isStrimsChat: state.ui.chatHost === CHAT_HOST_STRIMS,
      isServiceChat: state.ui.chatHost === CHAT_HOST_SERVICE,
      currentStreamService: idx(state, _ => _.streams[state.stream].service),
    }),
    { toggleChat }
  ),
)(Header);
