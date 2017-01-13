module.exports = {
  'parser': 'babel-eslint',
  'env': {
    'browser': true,
    'commonjs': true,
    'es6': true
  },
  'extends': [
    'eslint:recommended',
    'plugin:react/recommended',
  ],
  'installedESLint': true,
  'parserOptions': {
    'ecmaFeatures': {
      'impliedStrict': true,
      'experimentalObjectRestSpread': true,
      'jsx': true,
    },
    'sourceType': 'module'
  },
  'plugins': [
    'react',
  ],
  'rules': {
    'no-cond-assign': [
      'error',
      'always'
    ],
    'no-console': [
      'warn',
      { 'allow': ['warn', 'error'] },
    ],
    'no-constant-condition': [
      'error',
    ],
    'no-control-regex': [
      'error',
    ],
    'no-debugger': [
      'error',
    ],
    'no-dupe-args': [
      'error',
    ],
    'no-dupe-keys': [
      'error',
    ],
    'no-duplicate-case': [
      'error',
    ],
    'no-empty-character-class': [
      'error',
    ],
    'no-empty': [
      'error',
      { 'allowEmptyCatch': true },
    ],
    'no-ex-assign': [
      'error',
    ],
    'no-extra-boolean-cast': [
      'error',
    ],
    'no-extra-parens': [
      'off',
    ],
    'no-extra-semi': [
      'error',
    ],
    'no-func-assign': [
      'error',
    ],
    'no-inner-declarations': [
      'error',
    ],
    'no-invalid-regexp': [
      'error',
    ],
    'no-irregular-whitespace': [
      'error',
    ],
    'no-obj-calls': [
      'error',
    ],
    'no-prototype-builtins': [
      'off',
    ],
    'no-regex-spaces': [
      'error',
    ],
    'no-sparse-arrays': [
      'error',
    ],
    'no-template-curly-in-string': [
      'error',
    ],
    'no-unexpected-multiline': [
      'error',
    ],
    'no-unreachable': [
      'error',
    ],
    'no-unsafe-finally': [
      'error',
    ],
    'no-unsafe-negation': [
      'error',
    ],
    'use-isnan': [
      'error',
    ],
    'valid-jsdoc': [
      'off',
    ],
    'valid-typeof': [
      'error',
    ],
    'accessor-pairs': [
      'warn',
    ],
    'array-callback-return': [
      'off',
    ],
    'block-scoped-var': [
      'off',
    ],
    'class-methods-use-this': [
      'off',
    ],
    'complexity': [
      'error',
    ],
    'consistent-return': [
      'off',
    ],
    'curly': [
      'error',
    ],
    'default-case': [
      'off',
    ],
    'dot-location': [
      'error',
      'property',
    ],
    'dot-notation': [
      'error',
      { 'allowKeywords': true },
    ],
    'eqeqeq': [
      'error',
    ],
    'guard-for-in': [
      'off',
    ],
    'no-alert': [
      'warn',
    ],
    'no-caller': [
      'error',
    ],
    'no-case-declarations': [
      'error',
    ],
    'no-div-regex': [
      'off',
    ],
    'no-else-return': [
      'error',
    ],
    'no-empty-function': [
      'error',
    ],
    'no-empty-pattern': [
      'error',
    ],
    'no-eq-null': [
      'error',
    ],
    'no-eval': [
      'error',
    ],
    'no-extend-native': [
      'error',
    ],
    'no-extra-bind': [
      'error',
    ],
    'no-extra-label': [
      'error',
    ],
    'no-fallthrough': [
      'error',
    ],
    'no-floating-decimal': [
      'error',
    ],
    'no-global-assign': [
      'error',
    ],
    'no-implicit-coercion': [
      'error',
    ],
    'no-implicit-globals': [
      'off',
    ],
    'no-implied-eval': [
      'error',
    ],
    'no-invalid-this': [
      'error',
    ],
    'no-iterator': [
      'error',
    ],
    'no-labels': [
      'error',
    ],
    'no-lone-blocks': [
      'error',
    ],
    'no-loop-func': [
      'error',
    ],
    'no-magic-numbers': [
      'off',
      {
        'enforceConst': true
      },
    ],
    'no-multi-spaces': [
      'error',
    ],
    'no-multi-str': [
      'warn',
    ],
    'no-new-func': [
      'error',
    ],
    'no-new-wrappers': [
      'error',
    ],
    'no-new': [
      'error',
    ],
    'no-octal-escape': [
      'error',
    ],
    'no-octal': [
      'error',
    ],
    'no-param-reassign': [
      'warn',
      { 'props': false },
    ],
    'no-proto': [
      'error',
    ],
    'no-redeclare': [
      'error',
    ],
    'no-restricted-properties': [
      'off',
    ],
    'no-return-assign': [
      'off',
    ],
    'no-script-url': [
      'off',
    ],
    'no-self-assign': [
      'error',
    ],
    'no-self-compare': [
      'error',
    ],
    'no-sequences': [
      'warn',
    ],
    'no-throw-literal': [
      'error',
    ],
    'no-unmodified-loop-condition': [
      'error',
    ],
    'no-unused-expressions': [
      'error',
      { 'allowTernary': true },
    ],
    'no-unused-labels': [
      'error',
    ],
    'no-useless-call': [
      'error',
    ],
    'no-useless-concat': [
      'error',
    ],
    'no-useless-escape': [
      'error',
    ],
    'no-void': [
      'off',
    ],
    'no-warning-comments': [
      'warn',
    ],
    'no-with': [
      'error',
    ],
    'radix': [
      'error',
    ],
    'vars-on-top': [
      'error',
    ],
    'wrap-iife': [
      'error',
      'any',
    ],
    'yoda': [
      'error',
    ],
    'strict': [
      'error',
    ],
    'init-declarations': [
      'off',
    ],
    'no-catch-shadow': [
      'off',
    ],
    'no-delete-var': [
      'error',
    ],
    'no-label-var': [
      'error',
    ],
    'no-restricted-globals': [
      'off',
    ],
    'no-shadow-restricted-names': [
      'error',
    ],
    'no-shadow': [
      'off',
      {
        'builtinGlobals': false,
        'hoist': 'all',
        'allow': [],
      },
    ],
    'no-undef-init': [
      'warn',
    ],
    'no-undef': [
      'error',
    ],
    'no-undefined': [
      'off',
    ],
    'no-unused-vars': [
      'warn'
    ],
    'no-use-before-define': [
      'error',
    ],

    'callback-return': [
      'error',
    ],
    'global-require': [
      'off',
    ],
    'handle-callback-err': [
      'off',
    ],
    'no-mixed-requires': [
      'error',
    ],
    'no-new-require': [
      'error',
    ],
    'no-path-concat': [
      'error',
    ],
    'no-process-env': [
      'off',
    ],
    'no-process-exit': [
      'off',
    ],
    'no-restricted-modules': [
      'off',
    ],
    'no-sync': [
      'warn',
    ],

    'array-bracket-spacing': [
      'off',
    ],
    'block-spacing': [
      'error',
      'always',
    ],
    'brace-style': [
      'error',
      'stroustrup',
    ],
    'camelcase': [
      'off',
    ],
    'comma-dangle': [
      'error',
      'always-multiline',
    ],
    'comma-spacing': [
      'error',
      {
        'before': false,
        'after': true,
      }
    ],
    'comma-style': [
      'error',
      'last',
    ],
    'computed-property-spacing': [
      'error',
      'never',
    ],
    'consistent-this': [
      'off',
    ],
    'eol-last': [
      'error',
      'always',
    ],
    'func-call-spacing': [
      'error',
      'never',
    ],
    'func-names': [
      'error',
      'always',
    ],
    'func-style': [
      'off',
    ],
    'id-blacklist': [
      'off',
    ],
    'id-length': [
      'off',
    ],
    'id-match': [
      'off',
    ],
    'indent': [
      'error',
      2,
      {
        'SwitchCase': 1,
      },
    ],
    'jsx-quotes': [
      'error',
      'prefer-single',
    ],
    'key-spacing': [
      'error',
      {
        'beforeColon': false,
        'afterColon': true,
        'mode': 'strict',
      },
    ],
    'keyword-spacing': [
      'error',
      {
        'before': true,
        'after': true,
      },
    ],
    'line-comment-position': [
      'off',
    ],
    'linebreak-style': [
      'error',
      'unix'
    ],
    'lines-around-comment': [
      'off',
    ],
    'lines-around-directive': [
      'off',
    ],
    'max-depth': [
      'warn',
      10,
    ],
    'max-len': [
      'off',
    ],
    'max-lines': [
      'off',
    ],
    'max-nested-callbacks': [
      'warn',
      10,
    ],
    'max-params': [
      'off',
    ],
    'max-statements-per-line': [
      'off',
    ],
    'max-statements': [
      'off',
    ],
    'multiline-ternary': [
      'off',
    ],
    'new-cap': [
      'error',
      {
        'newIsCap': true,
        'capIsNew': false,
        'properties': true,
      },
    ],
    'new-parens': [
      'off',
    ],
    'newline-after-var': [
      'off',
    ],
    'newline-before-return': [
      'off',
    ],
    'newline-per-chained-call': [
      'error',
      { 'ignoreChainWithDepth': 4 },
    ],
    'no-array-constructor': [
      'error',
    ],
    'no-bitwise': [
      'off',
    ],
    'no-continue': [
      'off',
    ],
    'no-inline-comments': [
      'off',
    ],
    'no-lonely-if': [
      'off',
    ],
    'no-mixed-operators': [
      'off',
    ],
    'no-mixed-spaces-and-tabs': [
      'error',
    ],
    'no-multiple-empty-lines': [
      'off',
    ],
    'no-negated-condition': [
      'off',
    ],
    'no-nested-ternary': [
      'off',
    ],
    'no-new-object': [
      'error',
    ],
    'no-plusplus': [
      'off',
    ],
    'no-restricted-syntax': [
      'off',
    ],
    'no-tabs': [
      'error',
    ],
    'no-ternary': [
      'off',
    ],
    'no-trailing-spaces': [
      'error',
    ],
    'no-underscore-dangle': [
      'off',
    ],
    'no-unneeded-ternary': [
      'error',
    ],
    'no-whitespace-before-property': [
      'error',
    ],
    'object-curly-newline': [
      'off',
    ],
    'object-curly-spacing': [
      'error',
      'always',
    ],
    'object-property-newline': [
      'off',
    ],
    'one-var-declaration-per-line': [
      'off',
    ],
    'one-var': [
      'error',
      {
        'initialized': 'never',
        'uninitialized': 'always',
      },
    ],
    'operator-assignment': [
      'error',
      'always',
    ],
    'operator-linebreak': [
      'off',
      'none',
    ],
    'padded-blocks': [
      'off',
    ],
    'quote-props': [
      'off',
      'consistent-as-needed',
    ],
    'quotes': [
      'error',
      'single'
    ],
    'require-jsdoc': [
      'off',
    ],
    'semi-spacing': [
      'error',
      {'before': false, 'after': true},
    ],
    'semi': [
      'error',
      'always'
    ],
    'sort-keys': [
      'off',
    ],
    'sort-vars': [
      'off',
    ],
    'space-before-blocks': [
      'off',
    ],
    'space-before-function-paren': [
      'error',
      {'anonymous': 'always', 'named': 'never'},
    ],
    'space-in-parens': [
      'error',
      'never',
    ],
    'space-infix-ops': [
      'error',
    ],
    'space-unary-ops': [
      'error',
      {'words': true, 'nonwords': false},
    ],
    'spaced-comment': [
      'off',
    ],
    'unicode-bom': [
      'off',
    ],
    'wrap-regex': [
      'off',
    ],

    'arrow-body-style': [
      'off',
    ],
    'arrow-parens': [
      'error',
      'as-needed',
    ],
    'arrow-spacing': [
      'error',
       {'before': true, 'after': true},
    ],
    'constructor-super': [
      'error',
    ],
    'generator-star-spacing': [
      'error',
      'before',
    ],
    'no-class-assign': [
      'error',
    ],
    'no-confusing-arrow': [
      'off',
    ],
    'no-const-assign': [
      'error',
    ],
    'no-dupe-class-members': [
      'error',
    ],
    'no-duplicate-imports': [
      'error',
      { 'includeExports': true },
    ],
    'no-new-symbol': [
      'error',
    ],
    'no-restricted-imports': [
      'off',
    ],
    'no-this-before-super': [
      'error',
    ],
    'no-useless-computed-key': [
      'error',
    ],
    'no-useless-constructor': [
      'error',
    ],
    'no-useless-rename': [
      'error',
      {
        'ignoreDestructuring': false,
        'ignoreImport': false,
        'ignoreExport': false,
      },
    ],
    'no-var': [
      'error',
    ],
    'object-shorthand': [
      'off',
      'consistent-as-needed',
    ],
    'prefer-arrow-callback': [
      'error',
      {
        'allowNamedFunctions': true,
        'allowUnboundThis': true,
      }
    ],
    'prefer-const': [
      'error',
      {
        'destructuring': 'all',
        'ignoreReadBeforeAssign': false,
      }
    ],
    'prefer-numeric-literals': [
      'error',
    ],
    'prefer-reflect': [
      'off',
    ],
    'prefer-rest-params': [
      'error',
    ],
    'prefer-spread': [
      'error',
    ],
    'prefer-template': [
      'error',
    ],
    'require-yield': [
      'error',
    ],
    'rest-spread-spacing': [
      'error',
      'never',
    ],
    'sort-imports': [
      'off',
    ],
    'symbol-description': [
      'off',
    ],
    'template-curly-spacing': [
      'error',
      'never',
    ],
    'yield-star-spacing': [
      'error',
      'after',
    ],

    'react/display-name': [
      'off',
    ],
    'react/forbid-component-props': [
      'off',
    ],
    'react/forbid-prop-types': [
      'off',
    ],
    'react/no-children-prop': [
      'off',
    ],
    'react/no-danger': [
      'warn',
    ],
    'react/no-danger-with-children': [
      'error',
    ],
    'react/no-deprecated': [
      'warn',
    ],
    'react/no-did-mount-set-state': [
      'error',
    ],
    'react/no-did-update-set-state': [
      'error',
    ],
    'react/no-direct-mutation-state': [
      'error',
    ],
    'react/no-find-dom-node': [
      'error',
    ],
    'react/no-is-mounted': [
      'error',
    ],
    'react/no-multi-comp': [
      'error',
      { 'ignoreStateless': true },
    ],
    'react/no-render-return-value': [
      'error',
    ],
    'react/no-set-state': [
      'off',
    ],
    'react/no-string-refs': [
      'error',
    ],
    'react/no-unescaped-entities': [
      'off',
    ],
    'react/no-unknown-property': [
      'error',
    ],
    'react/no-unused-prop-types': [
      'warn',
    ],
    'react/prefer-es6-class': [
      'error',
      'always',
    ],
    'react/prefer-stateless-function': [
      'error',
    ],
    'react/prop-types': [
      'off',
    ],
    'react/react-in-jsx-scope': [
      'error',
    ],
    'react/require-optimization': [
      'off',
    ],
    'react/require-render-return': [
      'error',
    ],
    'react/self-closing-comp': [
      'error',
      {
        'component': true,
        'html': true,
      },
    ],
    'react/sort-comp': [
      'off',
    ],
    'react/sort-prop-types': [
      'off',
    ],
    'react/style-prop-object': [
      'error',
    ],


    'react/jsx-boolean-value': [
      'off',
    ],
    'react/jsx-closing-bracket-location': [
      'error',
      {
        'nonEmpty': false,
        'selfClosing': 'props-aligned',
      }
    ],
    'react/jsx-curly-spacing': [
      'error',
      'never',
    ],
    'react/jsx-equals-spacing': [
      'error',
      'never',
    ],
    'react/jsx-filename-extension': [
      'warn',
    ],
    'react/jsx-first-prop-new-line': [
      'error',
      'multiline',
    ],
    'react/jsx-handler-names': [
      'off',
    ],
    'react/jsx-indent': [
      'off',
      2,
    ],
    'react/jsx-indent-props': [
      'error',
      2,
    ],
    'react/jsx-key': [
      'warn',
    ],
    'react/jsx-max-props-per-line': [
      'error',
      { 'maximum': 20 },
    ],
    'react/jsx-no-bind': [
      'error',
      {
        'ignoreRefs': true,
        'allowArrowFunctions': true,
        'allowBind': false,
      },
    ],
    'react/jsx-no-comment-textnodes': [
      'warn',
    ],
    'react/jsx-no-duplicate-props': [
      'error',
    ],
    'react/jsx-no-literals': [
      'off',
    ],
    'react/jsx-no-target-blank': [
      'warn',
    ],
    'react/jsx-no-undef': [
      'error',
    ],
    'react/jsx-pascal-case': [
      'off',
    ],
    'react/jsx-sort-props': [
      'off',
    ],
    'react/jsx-space-before-closing': [
      'error',
    ],
    'react/jsx-uses-react': [
      'error',
    ],
    'react/jsx-uses-vars': [
      'error',
    ],
    'react/jsx-wrap-multilines': [
      'error',
      {
        'declaration': false,
        'assignment': false,
        'return': true,
      },
    ],

  }
};
