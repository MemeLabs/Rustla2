import React from 'react';
import { browserHistory } from 'react-router';


const handleSubmit = event => {
  event.preventDefault();
  const service = event.target.elements.service.value;
  const channel = event.target.elements.channel.value;
  if (channel && channel.length) {
    browserHistory.push(`/${service}/${channel}`);
  }
};

const HeaderForm = () => {
  return (
    <form
      className='navbar-form navbar-left'
      role='search'
      onSubmit={handleSubmit}
    >
      <select className='form-control' name='service'>
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
        <input
          className='form-control'
          placeholder='Stream/Video ID'
          type='text'
          name='channel'
          />
        <span className='input-group-btn'>
          <button type='submit' className='btn btn-default'>Go</button>
        </span>
      </div>
    </form>
  );
};

export default HeaderForm;
