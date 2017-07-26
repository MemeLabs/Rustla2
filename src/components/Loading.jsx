import React from 'react';


const Loading = ({ error, isLoading, pastDelay, timedOut }) => {
  if (isLoading) {
    if (timedOut) {
      return <div>Loader timed out</div>;
    }
    if (pastDelay) {
      return <div className='jiggle-background fill-percentage' />;
    }
    return null;
  }
  if (error) {
    return <p>Error!</p>;
  }
  return null;
};

export default Loading;
