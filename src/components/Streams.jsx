import React from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';

import '../css/Streams';
import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';


const Streams = ({ streams }) => {

  let grid = null;
  if (streams.length) {
    grid = streams.map(stream => {
      return (
        <div className='col-xs-12 col-sm-4 col-md-3 col-lg-2' key={stream.url}>
          <StreamThumbnail
            name={stream.channel}
            thumbnail={stream.image_url}
            url={stream.url}
            platform={stream.platform}
            viewers={stream.rustlers}
            />
        </div>
      );
    });
  }

  return (
    <MainLayout>
      <h1 className='streams-headling'>See what {streams.reduce((sum, stream) => sum + stream.rustlers, 0)} rustlers are watching!</h1>
      <div className='streams'>
        {grid}
      </div>
    </MainLayout>
  );
};

Streams.propTypes = {
  streams: React.PropTypes.array.isRequired,
};

export default compose(
  connect(state => ({ streams: state.streams })),
)(Streams);
