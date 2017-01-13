import React, { PropTypes } from 'react';

import '../css/MainLayout';

import Header from './Header';
import Footer from './Footer';


const MainLayout = ({ showHeader = true, showFooter = true, children }) =>
  <div className='main-layout'>
    {showHeader ? <Header /> : null}
    <div className='grow-1'>
      {children}
    </div>
    {showFooter ? <Footer /> : null}
  </div>
  ;

MainLayout.propTypes = {
  showHeader: PropTypes.bool,
  showFooter: PropTypes.bool,
  children: PropTypes.node.isRequired
};

export default MainLayout;
