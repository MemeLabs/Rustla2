// @flow

import React from 'react';

import '../css/NsfwStreamWarning';

type Props = {
  onAccept: () => void
};

const NsfwStreamWarning = ({ onAccept }: Props) =>
  <div className='nsfw-stream-warning'>
    <h1>Not Safe For Work!</h1>
    <p>
      This stream has been labelled Not Safe For Work (NSFW). Proceed with
      caution.
    </p>
    <br />
    <button type='button' className='btn btn-danger' onClick={onAccept}>
      Continue
    </button>
  </div>;

export default NsfwStreamWarning;
