import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { setStream } from '../actions';

import '../css/Streams';
import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';


const makeCategories = (categories, items) => {
  const sortedStreams = categories.map(() => []);
  for (const item of items) {
    for (let i = 0; i < categories.length; i++) {
      const { test } = categories[i];
      if (test(item)) {
        sortedStreams[i].push(item);
        break;
      }
    }
  }
  return categories.map(({ header }, i) =>
    sortedStreams[i].length ?
    <div key={i}>
      <h3 className='col-xs-12'>{header}</h3>
      {sortedStreams[i].map(stream => {
        // Don't send stream properties that aren't required by StreamThumbnail.
        // eslint-disable-next-line no-unused-vars
        const { created_at, updated_at, viewers, ...rest } = stream;

        return (
          <div className='col-xs-12 col-sm-4 col-md-3 col-lg-2' key={stream.id}>
            <StreamThumbnail {...rest} />
          </div>
        );
      }
      )}
    </div>
    : null
  );
};

const Streams = ({ streams }) => {
  let grid = null;
  const streams_arr = Object.values(streams);
  if (streams_arr.length) {
    grid = makeCategories([
      {
        header: 'Live OverRustle Streamers',
        test: stream => Boolean(stream.overrustle_id) && stream.live,
      },
      {
        header: 'Live Streams',
        test: stream => stream.live,
      },
      {
        header: 'Offline Streams',
        test: () => true,
      },
    ], streams_arr);
  }

  return (
    <MainLayout>
      <h1 className='streams-heading'>See what {streams_arr.reduce((sum, stream) => sum + stream.rustlers, 0)} rustlers are watching!</h1>
      <div className='streams flex-column grow-1'>{grid}</div>
    </MainLayout>
  );
};

Streams.propTypes = {
  streams: PropTypes.objectOf(
    PropTypes.shape({
      ...StreamThumbnail.propTypes,
      id: PropTypes.number.isRequired,
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
      document.title = 'OverRustle';
      this.props.setStream(null);
    },
  }),
)(Streams);
