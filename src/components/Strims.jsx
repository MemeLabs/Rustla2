import React from 'react';
import { connect } from 'react-redux';

import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';


const Strims = props => {
  const { streams } = props;

  let grid = null;
  if (streams.length) {
    grid = streams.map(stream => {
      return <div className='col-xs-12 col-sm-4 col-md-3 col-lg-2'>
        <StreamThumbnail name = {stream.channel}
          thumbnail = {stream.image_url} url = {stream.url}
          service = {stream.platform} viewers = {stream.rustlers}
          key = {stream.url}
        />
      </div>;
    });
  }

  return (
    <MainLayout>
      <div className='strims'>
        {grid}
      </div>
    </MainLayout>
  );
};

Strims.propTypes = {
  streams: React.PropTypes.array.isRequired,
};

function stateToProps(state) {
  return {
    streams: state.streams
  };
}

export default connect(stateToProps)(Strims);
