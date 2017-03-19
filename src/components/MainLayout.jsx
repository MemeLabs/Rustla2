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
  <div className='main-layout flex-column'>
    {showHeader ? <Header rustlerCount={rustlerCount} history={history} /> : null}
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
  history: PropTypes.object.isRequired,
};

export default MainLayout;
