import React, { PropTypes } from 'react';

import '../css/MainLayout';

import Header from './Header';
import Footer from './Footer';


const MainLayout = ({ showHeader = true, showFooter = true, rustlerCount, children }) =>
  <div className='main-layout flex-column'>
    {showHeader ? <Header rustlerCount={rustlerCount} /> : null}
    <div className='flex-column grow-1'>
      {children}
    </div>
    {showFooter ? <Footer /> : null}
  </div>
  ;

MainLayout.propTypes = {
  showHeader: PropTypes.bool,
  showFooter: PropTypes.bool,
  rustlerCount: Header.propTypes.rustlerCount,
  children: PropTypes.node.isRequired,
};

export default MainLayout;
