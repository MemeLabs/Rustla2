/* global GITHUB_URL DONATE_PAYPAL_URL DONATE_LINODE_URL DONATE_DO_URL */
import React from 'react';

import '../css/Footer';


const external_links = [
  {
    href: DONATE_PAYPAL_URL,
    children: 'Paypal',
  },
  {
    href: DONATE_LINODE_URL,
    children: 'Linode',
  },
  {
    href: DONATE_DO_URL,
    children: 'DigitalOcean',
  },
  {
    href: GITHUB_URL,
    children: 'GitHub',
  },
];

const Footer = () =>
  <footer>
    <div className='container'>
      <p className='text-muted'>
        OverRustle.com, a <a href='https://destiny.gg'>destiny.gg</a> community
        <br />
        By <a href={`${GITHUB_URL}/graphs/contributors`}>memers</a>
        <br />
        Support us via{'\u00a0'}
        {
          external_links
          .filter(({ href }) => href)
          .map((props, i) => <a key={i} {...props} target='_blank' rel='noopener noreferrer' />)
          .reduce((acc, curr, i, arr) => {
            acc.push(curr);
            if (i !== arr.length - 1) {
              acc.push(',\u00a0');
            }
            return acc;
          }, [])
        }
        <br />
        <a href='/api'>API</a>
      </p>
    </div>
  </footer>
  ;

export default Footer;
