/* global GITHUB_URL */
import React from 'react';
import PropTypes from 'prop-types';


const GitHubCommitLink = ({ hash, short }) =>
  <a href={`${GITHUB_URL}/commit/${hash}`}>
    {short}
  </a>;

GitHubCommitLink.propTypes = {
  hash: PropTypes.string.isRequired,
  short: PropTypes.string.isRequired,
};

export default GitHubCommitLink;
