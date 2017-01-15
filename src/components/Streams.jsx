import React from 'react';
import { connect } from 'react-redux';
import FlipMove from 'react-flip-move';

import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';
import '../css/Streams';


const Streams = props => {
  const { streams } = props;

  let grid = [];

  if (streams.length) {
    grid = streams.map(stream => {
      return (
        <div className='col-xs-12 col-sm-4 col-md-3 col-lg-2' key={stream.url}>
          <StreamThumbnail
            name={stream.name || stream.channel}
            thumbnail={stream.image_url}
            url={stream.url}
            service={stream.platform}
            viewers={stream.rustlers}
            />
        </div>
      );
    });
  }

  return (
    <MainLayout>
      <h1 className='streams-headling'>See what {
        streams.reduce((sum, stream) => sum + stream.rustlers, 0) } rustlers
        are watching!
      </h1>
      <div className='streams'>
        {
          // Cannot send an empty array of elements to `FlipMove`
          grid.length && <FlipMove>{grid}</FlipMove>
        }
      </div>
    </MainLayout>
  );
};

Streams.propTypes = {
  streams: React.PropTypes.array.isRequired,
};

function stateToProps(state) {
  return {
    streams: state.streams,
  };
}

export default connect(stateToProps)(Streams);
