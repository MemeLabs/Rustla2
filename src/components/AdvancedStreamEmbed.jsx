import React from 'react';
import PropTypes from 'prop-types';
import compose from 'recompose/compose';
import withHandlers from 'recompose/withHandlers';
import withState from 'recompose/withState';

import AdvancedStreamWarning from './AdvancedStreamWarning';

const AdvancedStreamEmbed = ({ acceptedWarning, acceptWarning, channel }) => {
  if (acceptedWarning) {
    return (
      <iframe
        width='100%'
        height='100%'
        marginHeight='0'
        marginWidth='0'
        frameBorder='0'
        scrolling='no'
        seamless
        allowFullScreen
        allowTransparency
        src={channel}
        />
    );
  }

  return <AdvancedStreamWarning onAccept={acceptWarning} channel={channel} />;
};

AdvancedStreamEmbed.propTypes = {
  acceptedWarning: PropTypes.bool.isRequired,
  acceptWarning: PropTypes.func.isRequired,
  channel: PropTypes.string.isRequired,
};

export default compose(
  withState('acceptedWarning', 'acceptWarning', false),
  withHandlers({
    acceptWarning: ({ acceptWarning }) => () => acceptWarning(true),
  })
)(AdvancedStreamEmbed);
