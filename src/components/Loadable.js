// @flow

import loadable, { type LoadingProps, type MapOptions, type Options } from 'react-loadable';

import Loading from './Loading';

type Module<TProps> =
  | React$ComponentType<TProps>
  | { default: React$ComponentType<TProps> };

type OptionsWithoutLoading<TProps, TModule>
  = $Diff<Options<TProps, TModule>, { loading: React$ComponentType<LoadingProps> }>;

type MapOptionsWithoutLoading<TProps, TModules>
  = $Diff<MapOptions<TProps, TModules>, { loading: React$ComponentType<LoadingProps> }>;

const Loadable = <TProps, TModule: Module<TProps>>(
  opts: OptionsWithoutLoading<TProps, TModule>
) =>
  loadable<TProps, TModule>({
    loading: Loading,
    ...opts,
  });

const LoadableMap = <TProps, TModules: { +[key: string]: * }>(
  opts: MapOptionsWithoutLoading<TProps, TModules>
) =>
  loadable.Map<TProps, TModules>({
    loading: Loading,
    ...opts,
  });

Loadable.Map = LoadableMap;

export default Loadable;
