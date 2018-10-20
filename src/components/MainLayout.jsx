import React from 'react';
import PropTypes from 'prop-types';

import '../css/MainLayout';

import Header from './Header';
import Footer from './Footer';


const MainLayout = ({
  showHeader = true,
  showFooter = true,
  rustlerCount,
  history,
  children,
}) =>
  <div className='main-layout d-flex flex-column'>
    {showHeader ? <Header rustlerCount={rustlerCount} history={history} /> : null}
    <div className='d-flex flex-column flex-grow-1'>
      {children}
    </div>
    {showFooter ? <Footer /> : null}
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
