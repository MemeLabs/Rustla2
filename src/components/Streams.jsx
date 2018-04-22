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

        // Find position to insert this stream at in order to keep this list
        // sorted.
        let position = 0;
        for (; position < sortedStreams[i].length; position++) {
          if (item.rustlers > sortedStreams[i][position].rustlers) {
            break;
          }
        }
        sortedStreams[i].splice(position, 0, item);
        break;
      }
    }
  }
  return categories.map(({ header }, i) =>
    sortedStreams[i].length ?
    <div key={i} className='streams'>
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

const Streams = ({ history, streams }) => {
  let grid = null;
  const streams_arr = Object.values(streams);
  if (streams_arr.length) {
    grid = makeCategories([
      {
        header: 'Live Community Streams',
        test: stream => (Boolean(stream.overrustle_id) || stream.service === 'angelthump') && stream.live,
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
    <MainLayout history={history}>
      <h1 className='streams-heading'>See what {streams_arr.reduce((sum, stream) => sum + stream.rustlers, 0)} rustlers are watching!</h1>
      <div className='flex-column grow-1'>{grid}</div>
    </MainLayout>
  );
};

Streams.propTypes = {
  history: PropTypes.object,
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
      document.title = 'Strims';
      this.props.setStream(null);
    },
  }),
)(Streams);
