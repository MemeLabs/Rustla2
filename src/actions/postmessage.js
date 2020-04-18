// @flow

/* global process */

import type { Store } from 'redux';

import history from '../history';

// thunks for these payloads
const thunks = {
  'STREAM_SET': ({path, channel, service}) => () => {
    history.push(path ? `/${path}` : `/${service}/${channel}`);
  },
};

export const init = (store: Store<*, *, *>) => {
  const handleMessage = (event) => {
    const origin = new URL(event.origin);
    if (!origin.hostname.endsWith(location.hostname)) {
      return;
    }

    if (process.env.NODE_ENV !== 'production') {
      // eslint-disable-next-line no-console
      console.log('window message', event.origin, event.data);
    }

    if (thunks[event.data.action]) {
      store.dispatch(thunks[event.data.action](event.data.payload));
    }
  };

  window.addEventListener('message', handleMessage, false);
};
