import React from 'react';
import PropTypes from 'prop-types';

import '../css/AdvancedStreamWarning';

const AdvancedStreamWarning = ({ channel, onAccept }) => {
  return (
    <div className='advanced-stream-warning'>
      <h1>WARNING</h1>
      <p>
        This "stream" requires embedding an external site onto this webpage.
        External webpages are outside of Strims.gg's control, and may serve
        malware, display not-safe-for-work content, and execute JavaScript, to
        name just a few of the dangers.
      </p>
      <p>
        By clicking "Accept", you are indicating that you understand the above
        risks, and that you have at least a reasonable level of trust in the
        following page:
      </p>
      <span><strong><code>{channel}</code></strong></span>
      <br />
      <button type='button' className='btn btn-danger' onClick={onAccept}>
        I Accept The Dangers
      </button>
    </div>
  );
};

AdvancedStreamWarning.propTypes = {
  channel: PropTypes.string.isRequired,
  onAccept: PropTypes.func.isRequired,
};

export default AdvancedStreamWarning;
