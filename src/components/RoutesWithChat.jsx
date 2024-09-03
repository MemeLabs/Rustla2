import React from 'react';
import { Router } from 'react-router-dom';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';
import history from '../history';
import Routes from '../routes';
import ChatEmbed from './ChatEmbed';
import Resizeable from './Resizeable';
import idx from 'idx';

import Header from './Header';
import Footer from './Footer';

import CustomScrollbar from './CustomScrollbar';

import '../css/Stream';

import {
  setChatSize,
  showChat,
  showHeader as headerFunc
} from '../actions';

export const RoutesWithChat = ({showHeader, showFooter, setChatSize, showChat, showLeftChat=false, chatClosed, chatSize, headerFunc}) =>
{
  let left = (
    <div className='flex-shrink-0 stream-embed' style={{ width: chatClosed ? '100%' : `calc(100% - ${chatSize}px)`, height: chatClosed ? '100%' :  '', display: 'flex', flexDirection: 'column'}}>
      <CustomScrollbar style={{ width: "100%", height: "100%"}}>
        <Routes />
      </CustomScrollbar>
      {showFooter ? <Footer /> : null}
    </div>
  );
  let right = chatClosed ? null : (
    <div className='chat-embed' style={{ width: chatSize, height: "inherit"}}>
      <ChatEmbed onClose={() => showChat(false)} />
    </div>
  );


  if (showLeftChat) {
    const temp = left;
    left = right;
    right = temp;
  }

  return (
    <div style={{height: "100%"}}>
      <Router history={history}>
        <div style={{ height: "100%", display: 'flex', flexDirection: 'column' }}>
          <Header history={history} classes={showHeader ? '' : 'close-header'} />
          <div title={showHeader ? "Close Header" : "Open Header"} className= {showHeader ?"close-header-btn" : 'open-header-btn'}>
            <span className={showHeader ? 'close-header-caret': 'open-header-caret'} onClick={() => showHeader ? headerFunc(false) : headerFunc(true)}>&#8250;</span>
          </div>
          <Resizeable
          className='flex-grow-1 flex-column flex-lg-row'
          onResize={e => {
          let newChatSize;
          if (showLeftChat) {
            newChatSize = e.pageX;
          }
          else {
            newChatSize = window.innerWidth - e.pageX;
          }
            setChatSize(newChatSize);
          }}>
          {left}
          {right}
          </Resizeable>
        </div>
      </Router>
    </div>
  );
};

RoutesWithChat.propTypes = {
  showHeader: PropTypes.bool,
  showFooter: PropTypes.bool,
  showLeftChat: PropTypes.bool,
  chatClosed: PropTypes.bool,

  chatSize: PropTypes.number.isRequired,

  setChatSize: PropTypes.func.isRequired,
  showChat: PropTypes.func.isRequired,
  headerFunc: PropTypes.func.isRequired,
};

export default compose(
  connect(
    state => ({
      showHeader: state.ui.showHeader,
      showFooter: state.ui.showFooter,
      chatSize: state.ui.chatSize,
      showLeftChat: idx(state, _ => _.self.profile.data.left_chat),
      chatClosed: !state.ui.showChat,
      headerClosed: !state.ui.showHeader,
    }),
    {
      setChatSize,
      showChat,
      headerFunc,
    },
  ),
)(RoutesWithChat);
