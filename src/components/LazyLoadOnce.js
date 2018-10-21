// @flow

import * as React from 'react';
import compose from 'recompose/compose';
import lifecycle from 'recompose/lifecycle';
import withState from 'recompose/withState';

type Props = {
  children?: React.Node,
  loaded?: boolean,
  visible?: boolean
};

/**
 * Helper component for lazily loading child components, but never unmounting
 * them. Will not render children until the `visible` property is set to `true`.
 * After this happens, the children will always be rendered regardless of
 * `visible`'s value.
 */
const LazyLoadOnce = ({ children, loaded, visible }: Props) => {
  if (loaded) {
    return children;
  }

  if (!visible) {
    return null;
  }

  return children;
};

export default compose(
  withState('loaded', 'setLoaded', false),
  lifecycle({
    componentDidUpdate() {
      // Ensure that we don't call `setLoaded(true)` if `loaded` is already
      // `true`, otherwise we will get stuck in an infinite loop of state
      // changes calling `componentDidUpdate()`.
      if (this.props.visible && !this.props.loaded) {
        this.props.setLoaded(true);
      }
    },
  }),
)(LazyLoadOnce);
