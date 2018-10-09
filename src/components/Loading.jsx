// @flow

import React from 'react';
import type { LoadingProps } from 'react-loadable';


const Loading = ({ error, pastDelay, timedOut }: LoadingProps) => {
  // If the loader errored, display generic error message.
  if (error) {
    return <p>Error!</p>;
  }

  // If the loader took too long, display generic timeout message.
  if (timedOut) {
    return <div>Loader timed out</div>;
  }

  // If the loader has taken long enough, then show the loading screen.
  if (pastDelay) {
    return <div className='jiggle-background fill-percentage' />;
  }

  // Loader has only just started; do not display anything yet.
  return null;
};

export default Loading;
