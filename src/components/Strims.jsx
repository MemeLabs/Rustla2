import React from 'react';

import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';


const Strims = props => {
  const { streams } = props;

  let grid = null;
  if (streams && streams.length) {
    grid = streams.map(stream => {
      return <StreamThumbnail name={stream.channel} thumbnail={stream.image_url}
        url={stream.url} service={stream.platform} viewers={stream.rustlers}
        key={stream.url}
      />;
    });
  }

  return <MainLayout>
    <div className='strims'>
      {grid}
    </div>
  </MainLayout>;
};

Strims.propTypes = {
  streams: React.PropTypes.array.isRequired
};

export default Strims;
