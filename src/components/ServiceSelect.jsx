import React from 'react';


const ServiceSelect = (props) =>
  <select className='form-control' name='service' {...props}>
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
  ;

export default ServiceSelect;
