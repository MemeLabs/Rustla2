import loadable from 'react-loadable';

import Loading from './Loading';


const Loadable = (opts) =>
  loadable({
    loading: Loading,
    ...opts,
  })
  ;

const LoadableMap = (opts) =>
  loadable.Map({
    loading: Loading,
    ...opts,
  })
  ;

Loadable.Map = LoadableMap;

export default Loadable;
