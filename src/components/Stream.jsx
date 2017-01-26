import React from 'react';
import { compose } from 'redux';
import { connect } from 'react-redux';
import lifecycle from 'recompose/lifecycle';

import { setStream } from '../actions';

import MainLayout from './MainLayout';
import Resizeable from './Resizeable';
import StreamEmbed from './StreamEmbed';


export const Stream = ({ channel, service }) =>
  <MainLayout showFooter={false}>
    <Resizeable className='grow-1'>
      <StreamEmbed channel={channel} service={service} />
      <iframe
        src='https://destiny.gg/embed/chat'
        frameBorder='0'
        width='100%'
        height='100%'
        />
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
