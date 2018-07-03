import React from 'react';
import PropTypes from 'prop-types';
import compose from 'recompose/compose';
import withHandlers from 'recompose/withHandlers';
import withState from 'recompose/withState';

import AdvancedStreamWarning from './AdvancedStreamWarning';

const ThirdPartyWarning = ({
  acceptedWarning,
  acceptWarning,
  channel,
  children,
}) =>
  acceptedWarning
    ? children
    : <AdvancedStreamWarning onAccept={acceptWarning} channel={channel} />;

ThirdPartyWarning.propTypes = {
  acceptedWarning: PropTypes.bool.isRequired,
  acceptWarning: PropTypes.func.isRequired,
  channel: PropTypes.string.isRequired,
  children: PropTypes.node,
};

export default compose(
  withState('acceptedWarning', 'acceptWarning', false),
  withHandlers({
    acceptWarning: ({ acceptWarning }) => () => acceptWarning(true),
  })
)(ThirdPartyWarning);
