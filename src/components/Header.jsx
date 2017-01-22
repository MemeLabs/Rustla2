/* global DONATE_PAYPAL_URL TWITCH_API_OAUTH_URL */
import React, { PropTypes } from 'react';
import { Link } from 'react-router';

import HeaderForm from './HeaderForm';
import '../css/Header';


// TODO - give this component `toggleSettings` dispatch-wrapped action that
// toggles the settings dropdown
const Header = ({ toggleSettings }) => {
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
};

export default Header;
