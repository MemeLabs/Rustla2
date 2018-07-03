import React from 'react';
import PropTypes from 'prop-types';

const AdvancedStreamEmbed = ({ channel }) =>
  <iframe
    width='100%'
    height='100%'
    marginHeight='0'
    marginWidth='0'
    frameBorder='0'
    scrolling='no'
    seamless
    allow='autoplay; fullscreen'
    allowFullScreen
    allowTransparency
    src={channel}
    />;

AdvancedStreamEmbed.propTypes = {
  channel: PropTypes.string.isRequired,
};

export default AdvancedStreamEmbed;
