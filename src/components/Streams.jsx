import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import setPropTypes from 'recompose/setPropTypes';
import lifecycle from 'recompose/lifecycle';
import idx from 'idx';


import {
  setStream,
  setChatSize,
  showChat,
} from '../actions';

import '../css/Streams';
import MainLayout from './MainLayout';
import StreamThumbnail from './StreamThumbnail';
import Resizeable from './Resizeable';
import ChatEmbed from './ChatEmbed';


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

const Streams = ({ history,
  streams,
  chatClosed,
  chatSize,
  setChatSize,
  showChat,
  showLeftChat = false,
}) => {
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
  let left = (
    <div style={{ width: chatClosed ? '100%' : `calc(100% - ${chatSize}px)`, marginBottom: 30 }}>
      <h1 className='streams-heading'>See what {streams_arr.reduce((sum, stream) => sum + stream.rustlers, 0)} rustlers are watching!</h1>
      {grid}
    </div>
  );
  let right = chatClosed ? null : (
    <div style={{ width: chatSize }}>
      <ChatEmbed onClose={() => showChat(false)} />
    </div>
  );
  if (showLeftChat) {
    const temp = left;
    left = right;
    right = temp;
  }

  return (
    <MainLayout history={history}>
      <Resizeable
        className='grow-1'
        onResize={e => {
          let newChatSize;
          if (showLeftChat) {
            newChatSize = e.pageX;
          }
          else {
            newChatSize = window.innerWidth - e.pageX;
          }
          setChatSize(newChatSize);
        }}
      >
        {left}
        {right}
      </Resizeable>
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
  chatClosed: PropTypes.bool,

  chatSize: PropTypes.number.isRequired,
  showLeftChat: PropTypes.bool,

  setChatSize: PropTypes.func.isRequired,
  showChat: PropTypes.func.isRequired,
};

export default compose(
  connect(
    state => ({
      streams: state.streams,
      chatSize: state.ui.chatSize,
      showLeftChat: idx(state, _ => _.self.profile.data.left_chat),
      chatClosed: !state.ui.showChat,
    }),
    {
      setStream,
      setChatSize,
      showChat,
    },
  ),
  setPropTypes({
    chatSize: PropTypes.number.isRequired,
    showLeftChat: PropTypes.bool,

    setChatSize: PropTypes.func.isRequired,
    showChat: PropTypes.func.isRequired,
  }),
  lifecycle({
    componentDidMount() {
      document.title = 'Strims';
      this.props.setStream(null);
    },
  }),
)(Streams);
