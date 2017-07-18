import React from 'react';
import PropTypes from 'prop-types';

import MainLayout from './MainLayout';


const Banned = ({ history }) =>
  <MainLayout history={history}>
    <div className='text-center'>
      <h1>Stream Banned</h1>
      <img src='/image/beand.jpg' className='img-top-margin' />
    </div>
  </MainLayout>
  ;

Banned.propTypes = {
  history: PropTypes.object.isRequired,
};

export default Banned;
