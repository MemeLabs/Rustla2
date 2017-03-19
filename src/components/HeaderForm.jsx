import React from 'react';
import PropTypes from 'prop-types';

import ServiceSelect from './ServiceSelect';


const HeaderForm = ({ history }) => {
  const handleSubmit = event => {
    event.preventDefault();
    const service = event.target.elements.service.value;
    const channel = event.target.elements.channel.value;
    if (channel && channel.length) {
      history.push(`/${service}/${channel}`);
    }
  };

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

HeaderForm.propTypes = {
  history: PropTypes.object.isRequired,
};

export default HeaderForm;
