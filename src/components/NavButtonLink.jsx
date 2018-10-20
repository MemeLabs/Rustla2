// @flow

import * as React from 'react';

type Props = {
  children?: React.Node
};

const NavButtonLink = ({ children }: Props) =>
  <a className='nav-link p-3' role='button'>{children}</a>;

export default NavButtonLink;
