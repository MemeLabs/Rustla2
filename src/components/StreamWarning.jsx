// @flow

import React from 'react';
import { compose, withStateHandlers } from 'recompose';
import type { Component } from 'recompose';

type WarningProps = {
  onAccept: () => void
};

type Props = {
  acceptedWarning: boolean,
  acceptWarning: () => void,
  stream: Component<*>,
  warning: Component<WarningProps>
};

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
