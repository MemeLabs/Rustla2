import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';
import idx from 'idx';

import {
  setStream,
  fetchProfileIfLoggedIn,
  showFooter,
} from '../actions';

import '../css/Streams';
import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';

const Thumbnail = stream => (
  <article key={stream.id}>
    <StreamThumbnail {...stream} />
  </article>
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
    <section className='px-4'>
      <h3 className='mt-4'>{header}</h3>
      <div className='streams-layout'>
        {thumbnails}
      </div>
    </section>
  );
};

Category.propTypes = {
  header: PropTypes.string.isRequired,
  streams: PropTypes.arrayOf(PropTypes.shape({
    promoted: PropTypes.bool,
    rustlers: PropTypes.number
  }))
};

const Streams = ({ history, streams, showHiddenStreams = false }) => {
  var visibleStreams = Object.values(streams);
  if (!showHiddenStreams)
    visibleStreams = visibleStreams.filter(({ hidden }) => !hidden);

  const categories = [
    {
      header: 'Live Community Streams',
      test: stream => (Boolean(stream.overrustle_id) || stream.service === 'angelthump') && stream.live && !stream.afk,
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
      <h1 className='streams-heading mt-4'>See what {viewerCount} rustlers are watching!</h1>
      <div className='d-flex flex-column'>{grid}</div>
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
  showHiddenStreams: PropTypes.bool
};

export default compose(
  connect(
    state => ({
      streams: state.streams,
      showHiddenStreams: idx(state, _ => _.self.profile.data.show_hidden),
    }),
    {
      setStream,
      fetchProfileIfLoggedIn,
      showFooter,
    },
  ),
  lifecycle({
    componentDidMount() {
      document.title = 'Strims';
      this.props.setStream(null);
      this.props.fetchProfileIfLoggedIn();
      this.props.showFooter(true);
    },
  }),
)(Streams);
