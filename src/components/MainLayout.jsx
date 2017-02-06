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

  // Should match `Header.propTypes.rustleCount`. Can't access
  // `Header.propTypes` here, however, because `Header` has been wrapped by
  // recompose. TODO: fix this if possible.
  rustlerCount: PropTypes.arrayOf(PropTypes.number),

  children: PropTypes.node.isRequired,
};

export default MainLayout;
