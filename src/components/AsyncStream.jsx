// @flow

// Global variable.
declare var API: string;

import React from 'react';
import lifecycle from 'recompose/lifecycle';
import { compose } from 'redux';
import type { History } from 'react-router';

import Error404 from './Error404';
import Loadable from './Loadable';

type Module<TProps> =
  // | React$ComponentType<TProps>
  | { default: React$ComponentType<TProps> };

type Modules = {
  Component: Module<*>,
  streamer: {
    channel: string,
    service: string
  }
};

type Props = {
  history: History,
  match: {
    params: {
      channel: string,
      service: string,
      streamer: string
    }
  }
};

const AsyncStream = ({
  history,
  match: { params: { service, channel, streamer } },
}: Props) => {
  const LoadableStream = React.useMemo((): React$ComponentType<any> => {
    return Loadable.Map<*, Modules>({
      loader: {
        Component: () => import(/* webpackChunkName: "stream" */ './Stream'),
        streamer: () => {
          // If this is the `/:service/:channel` route, then we do not need to
          // fetch any addition information from the server.
          if (service && channel) {
            return Promise.resolve();
          }

          // Otherwise, we're at the `/:streamer` route, and we need to fetch the
          // service and channel for this streamer.
          return fetch(`${API}/streamer/${streamer}`).then(res => res.json());
        },
      },
      render(loaded) {
        // If no service was provided, and none was found from the streamer
        // lookup, then render the 404 page.
        if (!service && !loaded.streamer.service) {
          return <Error404 history={history} />;
        }

        const Component = loaded.Component.default;
        return (
          <Component
            streamer={streamer}
            service={service || loaded.streamer.service}
            channel={channel || loaded.streamer.channel}
            history={history}
            />
        );
      },
    });
  }, [service, channel, streamer]);

  return <LoadableStream />;
};

export default compose(
  lifecycle({
    shouldComponentUpdate(nextProps){
      return nextProps.location.pathname != this.props.location.pathname;
   },
  }),
)(AsyncStream);
