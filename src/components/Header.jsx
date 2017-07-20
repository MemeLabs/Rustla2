/* global DONATE_PAYPAL_URL */
import React from 'react';
import PropTypes from 'prop-types';
import { Link } from 'react-router-dom';
import { compose } from 'redux';
import { connect } from 'react-redux';
import cs from 'classnames';
import setPropTypes from 'recompose/setPropTypes';
import get from 'lodash/get';

import '../css/Header';

import { toggleChat, CHAT_HOST_SERVICE, CHAT_HOST_STRIMS, CHAT_HOST_DGG } from '../actions';
import { supportedChatServices } from '../util/supported-chats';
import HeaderForm from './HeaderForm';


const Header = ({
  rustlerCount,
  isAdmin,
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
  if (rustlerCount) {
    const [ rCount, vCount ] = rustlerCount;
    if (rCount) {
      rustlers = <li><a>{rCount} Rustlers</a></li>;
    }
    if (vCount) {
      viewers = <li><a>{vCount} Viewers</a></li>;
    }
  }
  return (
    <nav
      className='header navbar navbar-default navbar-inverse'
      role='navigation'
    >
      <div className='container-fluid'>
        <div className='navbar-header'>
          <Link className='navbar-brand' to='/'>Strims</Link>
        </div>
        <div className='collapse navbar-collapse'>
          <ul className='nav navbar-nav'>
            {rustlers}
            {viewers}
            {DONATE_PAYPAL_URL ? <li><a target='_blank' rel='noopener noreferrer' href={DONATE_PAYPAL_URL}><span className='header-donate'>Donate</span></a></li> : null}
          </ul>
          <ul className='nav navbar-nav navbar-right'>
            {!currentStreamService ? null : <li onClick={() => toggleChat(CHAT_HOST_STRIMS)} className={cs({ active: isStrimsChat })}><a role='button'>Strims Chat</a></li>}
            {!currentStreamService ? null : <li onClick={() => toggleChat(CHAT_HOST_DGG)} className={cs({ active: isDggChat })}><a role='button'>Destiny Chat</a></li>}
            {!currentStreamService || !supportedChatServices.has(currentStreamService) ? null : <li onClick={() => toggleChat(CHAT_HOST_SERVICE)} className={cs('text-capitalize', { 'active': isServiceChat })}><a role='button'>{currentStreamService} Chat</a></li>}
            <li>
              <HeaderForm history={history} />
            </li>
            <li>
              <div className='btn-toolbar' role='toolbar'>
                <div className='btn-group'>
                  {
                    isAdmin
                      ? <Link className='btn btn-default navbar-btn' to='/admin' title='Admin'>
                        <span>Admin</span>
                      </Link>
                      : null
                  }
                </div>
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
            </li>
          </ul>
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
};

export default compose(
  setPropTypes({
    rustlerCount: PropTypes.arrayOf(PropTypes.number), // [rustlers, viewers] tuple
  }),
  connect(
    state => ({
      isAdmin: get(state, 'self.profile.data.is_admin', false),
      isLoggedIn: state.self.isLoggedIn,
      isDggChat: state.ui.chatHost === CHAT_HOST_DGG,
      isStrimsChat: state.ui.chatHost === CHAT_HOST_STRIMS,
      isServiceChat: state.ui.chatHost === CHAT_HOST_SERVICE,
      currentStreamService: get(state, ['streams', state.stream, 'service']),
    }),
    { toggleChat }
  ),
)(Header);
