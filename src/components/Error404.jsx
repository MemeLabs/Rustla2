import React, { PropTypes } from 'react';

import MainLayout from './MainLayout';


const Error404 = ({}) =>
  <MainLayout>
    <div className='text-center'>
      <h1>Strim Not Found</h1>
      <img src='/image/donger.png' />
      <h3>Whatever you are looking for, it's not here.</h3>
    </div>
  </MainLayout>
  ;

export default Error404;
