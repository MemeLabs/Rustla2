// @flow

const fnv1a = (input: string) => {
  let hash = 2166136261;
  for (let i = 0, len = input.length; i < len; i ++) {
    hash ^= input.charCodeAt(i);
    hash += (hash << 1) + (hash << 4) + (hash << 7) + (hash << 8) + (hash << 24);
  }
  return hash >>> 0;
};

const createRng = (seed: string) => {
  let n = 0;
  return () => fnv1a(`${n ++}${seed}`) / 0xffffffff;
};

export const generateColor = (seed: string) => {
  const rng = createRng(seed);
  const h = Math.round(rng() * 360);
  const s = Math.round(rng() * 80 + 20);
  const l = Math.round(rng() * 50 + 20);
  return `hsl(${h}, ${s}%, ${l}%)`;
};
