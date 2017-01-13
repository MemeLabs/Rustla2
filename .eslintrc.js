module.exports = {
  extends: 'semistandard',
  plugins: [
    'react'
  ],
  parserOptions: {
    ecmaVersion: 8,
    ecmaFeatures: {
      experimentalObjectRestSpread: true,
      jsx: true
    },
    sourceType: 'module'
  },
  rules: {
    'no-multiple-empty-lines': 'off',
    'no-return-assign': 'off',

    'jsx-quotes': ['error', 'prefer-single'],
    'react/jsx-boolean-value': 2,
    'react/jsx-curly-spacing': [2, 'never'],
    'react/jsx-equals-spacing': [2, 'never'],
    'react/jsx-indent': [2, 2],
    'react/jsx-indent-props': [2, 2],
    'react/jsx-no-duplicate-props': 2,
    'react/jsx-no-undef': 2,
    'react/jsx-space-before-closing': 2,
    'react/jsx-uses-react': 2,
    'react/jsx-uses-vars': 2,
    'react/self-closing-comp': 2,

    'react/jsx-no-bind': ['error', {
      // TODO: figure out why this is causing an error
      // allowArrowFunctons: true,
      allowBind: false,
      ignoreRefs: true
    }],
    'react/no-did-update-set-state': 'error',
    'react/no-unknown-property': 'error',
    'react/prop-types': 'error',
    'react/react-in-jsx-scope': 'error'
  },
  globals: {
    DONATE_DO_URL: false,
    DONATE_LINODE_URL: false,
    DONATE_PAYPAL_URL: false,
    GITHUB_URL: false,
    TWITCH_API_OAUTH_URL: false
  }
};
