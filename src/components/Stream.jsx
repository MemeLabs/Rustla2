import React from 'react';

import MainLayout from './MainLayout';
import Resizeable from './Resizeable';


const Stream = () =>
  <MainLayout showFooter={false}>
    <Resizeable className='grow-1'>
      <div style={{ backgroundColor: 'red', width: '50%' }} />
      <div style={{ backgroundColor: 'blue', width: '50%' }} />
    </Resizeable>
  </MainLayout>
  ;

export default Stream;