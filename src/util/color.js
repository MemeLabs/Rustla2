// @flow

import fnv1a from '@sindresorhus/fnv1a';

const createRng = (seed: string) => {
  let n = 0;
  return () => fnv1a(`${n ++}${seed}`) / 0xffffffff;
};

export const generateColor = (seed: string) => {
  const rng = createRng(seed);
  const h = Math.round(rng() * 360);
  const s = Math.round((rng() * 40) + 20);
  const l = Math.round((rng() * 40) + 20);
  return `hsl(${h}, ${s}%, ${l}%)`;
};
