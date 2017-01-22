import React from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { setStream } from '../actions';

import MainLayout from './MainLayout';
import Resizeable from './Resizeable';


export const Stream = () =>
  <MainLayout showFooter={false}>
    <Resizeable className='grow-1'>
      <div style={{ backgroundColor: 'red', width: '50%' }} />
      <div style={{ width: '50%' }}>
        <iframe
          src='https://destiny.gg/embed/chat'
          frameBorder='0'
          width='100%'
          style={{ height: '100%' }}
          />
      </div>
    </Resizeable>
  </MainLayout>
  ;

export default compose(
  connect(null, { setStream }),
  lifecycle({
    componentDidMount() {
      const { channel, service } = this.props.params;
      this.props.setStream(channel, service);
    },
  }),
)(Stream);
