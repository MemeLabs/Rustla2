// @flow

import loadable, { type MapOptions, type Options } from 'react-loadable';

import Loading from './Loading';

type Module<TProps> =
  | React$ComponentType<TProps>
  | { default: React$ComponentType<TProps> };

const Loadable = <TProps, TModule: Module<TProps>>(
  opts: Options<TProps, TModule>
) =>
  loadable<TProps, TModule>({
    loading: Loading,
    ...opts,
  });

const LoadableMap = <TProps, TModules: { +[key: string]: * }>(
  opts: MapOptions<TProps, TModules>
) =>
  loadable.Map<TProps, TModules>({
    loading: Loading,
    ...opts,
  });

Loadable.Map = LoadableMap;

export default Loadable;
