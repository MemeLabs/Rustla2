import React from 'react';
import PropTypes from 'prop-types';

import MainLayout from './MainLayout';


const Error404 = ({ history }) =>
  <MainLayout history={history}>
    <div className='text-center'>
      <h1>Strim Not Found</h1>
      <img src='/image/donger.png' />
      <h3>Whatever you are looking for, it&#x27;s not here.</h3>
    </div>
  </MainLayout>
  ;

Error404.propTypes = {
  history: PropTypes.object.isRequired,
};

export default Error404;
