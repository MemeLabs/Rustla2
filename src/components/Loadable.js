// @flow

import loadable from 'react-loadable';
import type { MapOptions, Options } from 'react-loadable';

import Loading from './Loading';


const Loadable = (opts: Options<*, *>) =>
  loadable({
    loading: Loading,
    ...opts,
  })
  ;

const LoadableMap = (opts: MapOptions<*, *>) =>
  loadable.Map({
    loading: Loading,
    ...opts,
  })
  ;

Loadable.Map = LoadableMap;

export default Loadable;
