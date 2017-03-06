/* global DONATE_PAYPAL_URL */
import React, { PropTypes } from 'react';
import { Link } from 'react-router';
import { compose } from 'redux';
import { connect } from 'react-redux';
import cs from 'classnames';
import setPropTypes from 'recompose/setPropTypes';
import get from 'lodash/get';

import '../css/Header';

import { toggleChat } from '../actions';
import HeaderForm from './HeaderForm';
import { supportedChatServices } from './ChatEmbed';


const Header = ({ rustlerCount, isLoggedIn, isOtherChatActive, currentStreamService, toggleChat }) => {
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
          <Link className='navbar-brand' to='/'>OverRustle</Link>
        </div>
        <div className='collapse navbar-collapse'>
          <ul className='nav navbar-nav'>
            {rustlers}
            {viewers}
            {DONATE_PAYPAL_URL ? <li><a target='_blank' rel='noopener noreferrer' href={DONATE_PAYPAL_URL}><span className='header-donate'>Donate</span></a></li> : null}
          </ul>
          <ul className='nav navbar-nav navbar-right'>
            {!currentStreamService || !supportedChatServices.has(currentStreamService) ? null : <li onClick={() => toggleChat(false)} className={cs({ active: !isOtherChatActive })}><a role='button'>Destiny Chat</a></li>}
            {!currentStreamService || !supportedChatServices.has(currentStreamService) ? null : <li onClick={() => toggleChat(true)} className={cs('text-capitalize', { 'active': isOtherChatActive })}><a role='button'>{currentStreamService} Chat</a></li>}
            <li>
              <HeaderForm />
            </li>
            <li>
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
            </li>
          </ul>
        </div>
      </div>
    </nav>
  );
};

Header.propTypes = {
  isLoggedIn: PropTypes.bool.isRequired,
  isOtherChatActive: PropTypes.bool.isRequired,
  currentStreamService: PropTypes.string,
  toggleChat: PropTypes.func.isRequired,
};

export default compose(
  setPropTypes({
    rustlerCount: PropTypes.arrayOf(PropTypes.number), // [rustlers, viewers] tuple
  }),
  connect(
    state => ({
      isLoggedIn: state.self.isLoggedIn,
      isOtherChatActive: state.ui.isOtherChatActive,
      currentStreamService: get(state, ['streams', state.stream, 'service']),
    }),
    { toggleChat }
  ),
)(Header);
