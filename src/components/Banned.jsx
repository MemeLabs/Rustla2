// @flow

import React from 'react';
import type { BrowserHistory } from 'history/createBrowserHistory';

import MainLayout from './MainLayout';

type Props = {
  history: BrowserHistory
};

const Banned = ({ history }: Props) =>
  <MainLayout history={history}>
    <div className='text-center'>
      <h1>Stream Banned</h1>
      <img src='/image/beand.jpg' className='img-top-margin' />
    </div>
  </MainLayout>;

export default Banned;
