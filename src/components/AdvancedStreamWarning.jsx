import React from 'react';
import PropTypes from 'prop-types';

import '../css/AdvancedStreamWarning';

const AdvancedStreamWarning = ({ channel, onAccept }) => {
  return (
    <div className='advanced-stream-warning'>
      <h1>WARNING</h1>
      <p>
        This stream embed is served by an untrusted 3rd party site. It may
        contain obscene (NSFW/NSFL) content, execute JavaScript, or record
        your IP address.
      </p>
      <p>
        By clicking "Continue", you are indicating that you understand the
        above risks, and have reasonable trust in the following page:
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
