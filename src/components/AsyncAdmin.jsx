import React from 'react';

import Loadable from './Loadable';


const AsyncStream = Loadable({
  loader: () => import(/* webpackChunkName: "admin" */ './Admin'),
});

export default AsyncStream;
