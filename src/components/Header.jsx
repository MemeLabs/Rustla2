/* global DONATE_PAYPAL_URL TWITCH_API_OAUTH_URL */
import React, { PropTypes } from 'react';
import { Link } from 'react-router';

import '../css/Header';

import HeaderForm from './HeaderForm';


// TODO - give this component `toggleSettings` dispatch-wrapped action that
// toggles the settings dropdown
const Header = ({ toggleSettings, rustlerCount }) => {
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
        <div className='navbar-header hidden-sm'>
          <Link className='navbar-brand' to='/'>OverRustle</Link>
        </div>
        <div className='collapse navbar-collapse'>
          <ul className='nav navbar-nav'>
            {rustlers}
            {viewers}
            <li>
              <a target='_blank' rel='noopener noreferrer' href={DONATE_PAYPAL_URL} />
            </li>
          </ul>
          <ul className='nav navbar-nav navbar-right'>
            <li>
              <HeaderForm />
            </li>
            <li>
              <div className='btn-group'>
                <a className='btn btn-default navbar-btn' rel='noopener noreferrer' href={TWITCH_API_OAUTH_URL} title='Log In'>
                  <span className='glyphicon glyphicon-log-in' />
                </a>
                <button className='btn btn-default navbar-btn' title='Settings' type='button' onClick={toggleSettings}>
                  <span className='glyphicon glyphicon-cog' />
                </button>
              </div>
            </li>
          </ul>
        </div>
      </div>
    </nav>
  );
};

Header.propTypes = {
  toggleSettings: PropTypes.func.isRequired,
  rustlerCount: PropTypes.arrayOf(PropTypes.number), // [rustlers, viewers] tuple
};

export default Header;
