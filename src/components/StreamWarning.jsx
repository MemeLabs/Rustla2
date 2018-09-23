// @flow

import React from 'react';
import type { ComponentType } from 'react';
import { compose, withStateHandlers } from 'recompose';

type WarningProps = {
  onAccept: () => void
};

type Props = {
  acceptedWarning: boolean,
  acceptWarning: () => void,
  stream: ComponentType<{||}>,
  warning: ComponentType<WarningProps>
};

// $FlowFixMe
const StreamWarning = ({ acceptedWarning, acceptWarning, stream, warning }: Props) =>
  acceptedWarning
    ? React.createElement(stream)
    : React.createElement(warning, { onAccept: acceptWarning });

export default compose(
  withStateHandlers(
    () => ({ acceptedWarning: false }),
    {
      acceptWarning: () => () => ({ acceptedWarning: true })
    }
  )
)(StreamWarning);
