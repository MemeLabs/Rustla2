import React, { PropTypes } from 'react';
import { Link, browserHistory } from 'react-router';

import '../css/Header';

const onHeaderFormSubmit = e => {
  console.log(e);
  // TODO - navigate to correct location with `browserHistory`
};

// TODO - give this component `toggleSettings` dispatch-wrapped action that toggles the settings dropdown
const Header = ({ toggleSettings = noop => noop }) =>
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
          <li className='hidden-md hidden-sm'>
            <form className='navbar-form navbar-left' role='search' onSubmit={onHeaderFormSubmit}>
              <select className='form-control'>
                <option value='angelthump'>AngelThump</option>
                <option value='azubu'>azubu</option>
                <option value='dailymotion'>Dailymotion</option>
                <option value='facebook'>Facebook</option>
                <option value='hitbox'>Hitbox</option>
                <option value='hitbox-vod'>hitbox (VOD)</option>
                <option value='mlg'>MLG</option>
                <option value='nsfw-chaturbate'>Chaturbate (NSFW)</option>
                <option value='streamup'>StreamUp</option>
                <option value='twitch'>Twitch</option>
                <option value='twitch-vod'>Twitch (VOD)</option>
                <option value='ustream'>Ustream</option>
                <option value='vaughn'>Vaughn</option>
                <option value='youtube'>YouTube</option>
                <option value='youtube-playlist'>YouTube (playlist)</option>
                <option value='advanced'>Advanced</option>
              </select>
              <div className='input-group'>
                <input className='form-control' placeholder='Stream/Video ID' />
                <span className='input-group-btn'>
                  <button type='submit' className='btn btn-default hidden-md hidden-sm'>Go</button>
                </span>
              </div>
            </form>
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
  ;

Header.propTypes = {
  toggleSettings: PropTypes.func.isRequired,
};

export default Header;
