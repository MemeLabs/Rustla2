// @flow

// Global
declare var GITHUB_URL;

import React from 'react';

type Props = {
  hash: string,
  short: string
};

const GitHubCommitLink = ({ hash, short }: Props) =>
  <a href={`${GITHUB_URL}/commit/${hash}`}>
    {short}
  </a>;

export default GitHubCommitLink;
