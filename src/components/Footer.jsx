import React, { PropTypes } from 'react';

import 'bootstrap/dist/css/bootstrap.css';
import '../css/Footer';

const Footer = ({}) =>
  <footer>
    <div className='container'>
      <p className='text-muted'>
        OverRustle.com, a <a href='https://destiny.gg'>destiny.gg</a> community
        <br />
        By <a href={`${GITHUB_URL}/graphs/contributors`}>memers</a>
      <br />
      Support us via{'\u00a0'}
      <a href={DONATE_PAYPAL_URL} target='_blank' rel='noopener noreferrer'>Paypal</a>,{'\u00a0'}
        <a href={DONATE_LINODE_URL} target='_blank' rel='noopener noreferrer'>Linode</a>,{'\u00a0'}
          <a href={DONATE_DO_URL} target='_blank' rel='noopener noreferrer'>DigitalOcean</a>,{'\u00a0'}
            <a href={GITHUB_URL} target='_blank' rel='noopener noreferrer'>GitHub</a>
            <br />
            <a href='/api'>API</a>
          </p>
    </div>
  </footer>
  ;

export default Footer;
