/* global GITHUB_URL DONATE_PAYPAL_URL DONATE_LINODE_URL DONATE_DO_URL
          DISCORD_URL GIT_COMMIT_HASH GIT_SHORT_COMMIT_HASH CHAT_URL */
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
