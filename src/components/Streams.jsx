import React, { PropTypes } from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { setStream } from '../actions';

import '../css/Streams';
import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';


const Streams = ({ streams }) => {

  let grid = null;
  if (streams.length) {
    grid = streams.map(stream => {
      return (
        <div className='col-xs-12 col-sm-4 col-md-3 col-lg-2' key={stream.id}>
          <StreamThumbnail {...stream} />
        </div>
      );
    });
  }

  return (
    <MainLayout>
      <h1 className='streams-headling'>See what {Object.values(streams).reduce((sum, stream) => sum + stream.rustlers, 0)} rustlers are watching!</h1>
      <div className='streams'>
        {grid}
      </div>
    </MainLayout>
  );
};

Streams.propTypes = {
  streams: PropTypes.objectOf(
    PropTypes.shape({
      id: PropTypes.string.isRequired,
      rustlers: PropTypes.number.isRequired,
    }).isRequired,
  ).isRequired,
};

export default compose(
  connect(
    state => ({ streams: state.streams }),
    { setStream },
  ),
  lifecycle({
    componentDidMount() {
      this.props.setStream(null);
    },
  }),
)(Streams);
