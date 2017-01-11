import React from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import { Link } from 'react-router';


const App = props =>
  <div>
    <div>
      You are at {props.location.pathname}
    </div>
    <div><Link to='/one'>one</Link></div>
    <div><Link to='/two'>two</Link></div>
    <div><Link to='/three'>three</Link></div>
  </div>
  ;

export default compose(
  connect(
    state => state,
  ),
)(App);
