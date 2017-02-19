import React from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';


export const LoadingScreen = ({ isLoading, children }) => {
  if (isLoading) {
    return <div className='jiggle-background fill-percentage' />;
  }
  return children;
};

export default compose(
  connect(
    state => ({
      isLoading: state.isLoading,
    }),
  ),
)(LoadingScreen);
