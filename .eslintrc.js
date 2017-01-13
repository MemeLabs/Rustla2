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
    'react/jsx-boolean-value': 'error',
    'react/jsx-curly-spacing': ['error', 'never'],
    'react/jsx-equals-spacing': ['error', 'never'],
    'react/jsx-indent': ['error', 2],
    'react/jsx-indent-props': ['error', 2],
    'react/jsx-no-duplicate-props': 'error',
    'react/jsx-no-undef': 'error',
    'react/jsx-space-before-closing': 'error',
    'react/jsx-uses-react': 'error',
    'react/jsx-uses-vars': 'error',
    'react/self-closing-comp': 'error',

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
