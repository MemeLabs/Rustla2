import React, { PropTypes } from 'react';
import { Link } from 'react-router';

import MainLayout from './MainLayout';


const Strims = props =>
  <MainLayout>
    <div>
      You are at {props.location.pathname}
    </div>
    <div><Link to='/one'>one</Link></div>
    <div><Link to='/two'>two</Link></div>
    <div><Link to='/three'>three</Link></div>
  </MainLayout>
  ;

export default Strims;
