import React from 'react';
import { Field, reduxForm } from 'redux-form';
import { browserHistory } from 'react-router';


const HeaderForm = ({ handleSubmit }) => {
  return (
    <form
      className='navbar-form navbar-left'
      role='search'
      onSubmit={handleSubmit}
    >
      <Field
        className='form-control'
        component='select'
        name='service'
      >
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
      </Field>
      <div className='input-group'>
        <Field
          className='form-control'
          placeholder='Stream/Video ID'
          component='input'
          name='channel'
          type='text'
          />
        <span className='input-group-btn'>
          <button type='submit' className='btn btn-default'>Go</button>
        </span>
      </div>
    </form>
  );
};

HeaderForm.propTypes = {
  handleSubmit: React.PropTypes.func.isRequired,
};

export default reduxForm({
  form: 'header',
  initialValues: {
    service: 'angelthump',
    channel: null,
  },
  onSubmit: data => {
    // don't do anything if the channel field is empty
    if (!data.channel.length) {
      return;
    }

    browserHistory.push(`/${data.service}/${data.channel}`);
  },
})(HeaderForm);
