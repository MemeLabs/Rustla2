// @flow

// Globals
declare var CHAT_URL: string;
declare var DISCORD_URL: string;
declare var DONATE_DO_URL: string;
declare var DONATE_LINODE_URL: string;
declare var DONATE_PAYPAL_URL: string;
declare var GIT_COMMIT_HASH: string;
declare var GIT_SHORT_COMMIT_HASH: string;
declare var GITHUB_URL: string;

import React from 'react';

import '../css/Footer';
import GitHubCommitLink from './GitHubCommitLink';


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
        Strims.gg
        <br />
        By <a href={`${GITHUB_URL}/graphs/contributors`}>memers</a> on <a href={`${DISCORD_URL}`}>Discord</a>
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
        {
          CHAT_URL &&
            <React.Fragment>
              <a href={CHAT_URL}>Chat</a>
              &nbsp; &bull; &nbsp;
            </React.Fragment>
        }
        <a href='/api'>API</a>
        &nbsp; &bull; &nbsp;
        <GitHubCommitLink hash={GIT_COMMIT_HASH} short={GIT_SHORT_COMMIT_HASH} />
      </p>
    </div>
  </footer>
  ;

export default Footer;
