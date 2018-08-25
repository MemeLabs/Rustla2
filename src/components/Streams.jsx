import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { setStream } from '../actions';

import '../css/Streams';
import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';

const Thumbnail = stream => (
  <div className='col-xs-12 col-sm-4 col-md-3 col-lg-2' key={stream.id}>
    <StreamThumbnail {...stream} />
  </div>
);

const Category = ({ header, streams }) => {
  const thumbnails = streams
    .sort((a, b) => {
      if (a.promoted !== b.promoted) {
        return a.promoted ? -1 : 1;
      }
      return b.rustlers - a.rustlers;
    })
    .map((stream, i) => <Thumbnail key={i} {...stream} />);

  return (
    <div className='streams'>
      <h3 className='col-xs-12'>{header}</h3>
      {thumbnails}
    </div>
  );
};

const Streams = ({ history, streams }) => {
  const visibleStreams = Object.values(streams).filter(({ hidden }) => !hidden);

  const categories = [
    {
      header: 'Live Community Streams',
      test: stream => (Boolean(stream.overrustle_id) || stream.service === 'angelthump') && stream.live,
      streams: [],
    },
    {
      header: 'Live Streams',
      test: stream => stream.live,
      streams: [],
    },
    {
      header: 'Offline Streams',
      test: () => true,
      streams: [],
    },
  ];

  const reduceCategories = (categories, item) => {
    const i = categories.findIndex(({ test }) => test(item));
    categories[i].streams.push(item);
    return categories;
  };

  const grid = visibleStreams
    .reduce(reduceCategories, categories)
    .filter(({ streams }) => streams.length > 0)
    .map((category, i) => <Category key={i} {...category} />);

  const viewerCount = visibleStreams
    .reduce((sum, stream) => sum + stream.rustlers, 0)
    .toLocaleString();

  return (
    <MainLayout history={history}>
      <h1 className='streams-heading'>See what {viewerCount} rustlers are watching!</h1>
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
