import React from 'react';
import PropTypes from 'prop-types';
import classNames from 'classnames';
import compose from 'recompose/compose';
import withHandlers from 'recompose/withHandlers';
import withState from 'recompose/withState';

const Checkbox = ({
  checked,
  className,

  // Intentionally remove `defaultChecked` and `setChecked` from `miscProps`.
  /* eslint-disable no-unused-vars */
  defaultChecked,
  setChecked,
  /* eslint-enable no-unused-vars */

  toggle,
  ...miscProps
}) => {
  const classes = classNames(className, 'form-check-input');

  return (
    <input
      type='checkbox'
      checked={checked}
      className={classes}
      onChange={toggle}
      {...miscProps}
      />
  );
};

Checkbox.propTypes = {
  checked: PropTypes.bool,
  className: PropTypes.string,
  defaultChecked: PropTypes.bool,
  setChecked: PropTypes.func,
  toggle: PropTypes.func,
};

export default compose(
  withState('checked', 'setChecked', ({ defaultChecked }) => defaultChecked),
  withHandlers({
    toggle: ({ setChecked }) => () => setChecked(current => !current),
  })
)(Checkbox);
