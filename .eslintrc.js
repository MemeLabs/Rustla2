module.exports = {
  parser: 'babel-eslint',
  env: {
    browser: true,
    commonjs: true,
    es6: true,
  },
  extends: [
    'eslint:recommended',
    'plugin:flowtype/recommended',
    'plugin:react/recommended',
  ],
  parserOptions: {
    ecmaFeatures: {
      impliedStrict: true,
      experimentalObjectRestSpread: true,
      jsx: true,
    },
    sourceType: 'module',
  },
  plugins: [
    'flowtype',
    'react',
  ],
  rules: {
    // eslint rules
    'no-console': 'warn',
    'no-unused-vars': 'warn',

    // eslint-plugin-react rules
    'react/no-unescaped-entities': 'warn',
    'react/prop-types': 'warn',
  }
};
