import React from 'react';
import PropTypes from 'prop-types';

import '../css/AdvancedStreamWarning';

const AdvancedStreamWarning = ({ channel, onAccept }) => {
  return (
    <div className='advanced-stream-warning'>
      <h1>EMBED NOTICE</h1>
      <p>
        This is a custom stream embed. It may contain obscene
        (NSFW/NSFL) content, or record your IP address. Continue?
      </p>
      <span><strong><code>{channel}</code></strong></span>
      <br />
      <button type='button' className='btn btn-danger' onClick={onAccept}>
        Continue
      </button>
    </div>
  );
};

AdvancedStreamWarning.propTypes = {
  channel: PropTypes.string.isRequired,
  onAccept: PropTypes.func.isRequired,
};

export default AdvancedStreamWarning;
