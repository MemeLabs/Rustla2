import React, { PropTypes } from 'react';

import '../css/MainLayout';

import Header from './Header';
import Footer from './Footer';


const MainLayout = ({ showHeader = true, showFooter = true, children }) =>
  <div className='main-layout flex-column'>
    {showHeader ? <Header /> : null}
    <div className='flex-column grow-1'>
      {children}
    </div>
    {showFooter ? <Footer /> : null}
  </div>
  ;

MainLayout.propTypes = {
  showHeader: PropTypes.bool,
  showFooter: PropTypes.bool,
  children: PropTypes.node.isRequired,
};

export default MainLayout;
