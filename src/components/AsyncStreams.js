// @flow

import Loadable from './Loadable';


const AsyncStreams = Loadable({
  loader: () => import(/* webpackChunkName: "streams" */ './Streams'),
});

export default AsyncStreams;
