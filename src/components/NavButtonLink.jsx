// @flow

import * as React from 'react';

type Props = {
  children?: React.Node
};

const NavButtonLink = ({ children }: Props) => (
  <a className="nav-link p-3 pointer" role='button'>{children}</a>
);

export default NavButtonLink;
