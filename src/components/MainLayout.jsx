import React from 'react';
import PropTypes from 'prop-types';

import '../css/MainLayout';

const MainLayout = ({
  children,
}) =>
  <div className='main-layout d-flex '>
    <div className='d-flex flex-column flex-grow-1' style={{width: '100%'}}>
      {children}
    </div>
  </div>
  ;

MainLayout.propTypes = {
  showHeader: PropTypes.bool,
  showFooter: PropTypes.bool,
  rustlerCount: PropTypes.arrayOf(PropTypes.number),
  children: PropTypes.node.isRequired,
  history: PropTypes.object.isRequired,
};

export default MainLayout;
