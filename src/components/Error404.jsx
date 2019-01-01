// @flow

import React from 'react';
import type { BrowserHistory } from 'history/createBrowserHistory';

import MainLayout from './MainLayout';

type Props = {
  history: BrowserHistory
};

const Error404 = ({ history }: Props) =>
  <MainLayout history={history}>
    <div className='text-center'>
      <h1>Strim Not Found</h1>
      <img src='/image/donger.png' />
      <h3>Whatever you are looking for, it&#x27;s not here.</h3>
    </div>
  </MainLayout>
  ;

export default Error404;
