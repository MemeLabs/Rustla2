import React, { PropTypes } from 'react';

import MainLayout from './MainLayout';
import Resizeable from './Resizeable';


const Strim = ({}) =>
  <MainLayout showFooter={false}>
    <Resizeable className='grow-1'>
      <div className='grow-1' style={{ backgroundColor: 'red', width: '50%' }} />
      <div className='grow-1' style={{ backgroundColor: 'blue', width: '50%' }} />
    </Resizeable>
  </MainLayout>
  ;

export default Strim;
