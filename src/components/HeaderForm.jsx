import React from 'react';
import { browserHistory } from 'react-router';

import ServiceSelect from './ServiceSelect';


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
      <ServiceSelect
        style={{
          marginRight: -1,
          borderBottomRightRadius: 0,
          borderTopRightRadius: 0,
        }}
        />
      <div className='input-group'>
        <input
          className='form-control'
          placeholder='Stream/Video ID'
          type='text'
          name='channel'
          style={{
            borderTopLeftRadius: 0,
            borderBottomLeftRadius: 0,
          }}
          />
        <span className='input-group-btn'>
          <button type='submit' className='btn btn-default'>Go</button>
        </span>
      </div>
    </form>
  );
};

export default HeaderForm;
