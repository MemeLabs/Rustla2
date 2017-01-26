/* global process __dirname */
/* eslint quotes: 0 */
/* eslint no-template-curly-in-string: 0 */
require('dotenv').config({ silent: true });

const webpack = require('webpack');
const path = require('path');
const ExtractTextPlugin = require('extract-text-webpack-plugin');

const { NODE_ENV } = process.env;
const IS_PRODUCTION = NODE_ENV === 'production';
console.log(`Bundling for ${(NODE_ENV || 'development').toUpperCase()}`);


module.exports = {
  entry: {
    main: [
      'bootstrap/dist/css/bootstrap.css',
      'babel-polyfill',
      'isomorphic-fetch',
      path.resolve(__dirname, 'src/css/main.scss'),
      path.resolve(__dirname, 'src/client.jsx'),
    ],
  },
  output: {
    path: path.join(__dirname, './public/assets'),
    filename: IS_PRODUCTION ? '[name].[chunkhash].js' : '[name].js',
    publicPath: '/assets',
    sourceMapFilename: '[name].[chunkhash].js.map',
    crossOriginLoading: 'anonymous',
  },
  devServer: {
    contentBase: path.join(__dirname, './public'),
    host: '0.0.0.0',
    port: process.env.PORT || 3000,
    historyApiFallback: true,
    watchOptions: {
      aggregateTimeout: 100,
    },
  },
  debug: !IS_PRODUCTION,
  cache: true,
  stats: {
    colors: true,
  },
  resolve: {
    extensions: ['', '.js', '.jsx', '.scss', '.json'],
  },
  module: {
    preLoaders: [
      {
        test: /\.jsx?$/,
        loader: 'eslint',
      },
    ],
    loaders: [
      {
        test: /\.jsx?$/,
        include: path.resolve(__dirname, 'src'),
        loader: 'babel',
        query: {
          babelrc: false,
          presets: [
            require.resolve('babel-preset-es2015'),
            require.resolve('babel-preset-react'),
            require.resolve('babel-preset-stage-0'),
          ],
          cacheDirectory: true,
        },
      },
      {
        test: /\.json/,
        loaders: ['json'],
      },
      {
        test: /\.s?css$/,
        loader: ExtractTextPlugin.extract('style', 'css?importLoaders=1!postcss!sass'),
      },
      {
        test: /\.(eot|woff|woff2|ttf|png|jpg|svg|gif)/,
        loaders: ['url?limit=30000&name=[name]-[hash].[ext]'],
      },
    ],
  },
  eslint: {
    configFile: path.resolve(__dirname, '.eslintrc.js'),
  },
  postcss: [
    require('autoprefixer')({ browsers: ['last 2 versions'] }),
  ],
  plugins: [
    new (require('html-webpack-plugin'))({
      template: './src/index.ejs',
      filename: '../index.html',
      chunksSortMode: 'none',
      inject: true,
      minify: {
        minifyCSS: true,
        minifyJS: true,
        collapseWhitespace: IS_PRODUCTION,
      },
    }),
    new (require('webpack-subresource-integrity'))({
      hashFuncNames: ['sha256', 'sha384'],
      enabled: IS_PRODUCTION,
    }),
    new webpack.DefinePlugin(Object.assign({
      'process.env.NODE_ENV': `"${NODE_ENV}"`,
      'API': process.env.API ? `"${process.env.API}"` : "'/api'",
      'API_WS': process.env.API_WS ? `"${process.env.API_WS}"` : undefined,
      'GITHUB_URL': process.env.GITHUB_URL ? `"${process.env.GITHUB_URL}"` : "'https://github.com/ILiedAboutCake/Rustla2'",
      'DONATE_PAYPAL_URL': process.env.DONATE_PAYPAL_URL ? `"${process.env.DONATE_PAYPAL_URL}"` : "'/nicememe'",
      'DONATE_LINODE_URL': process.env.DONATE_LINODE_URL ? `"${process.env.DONATE_LINODE_URL}"` : "'/nicememe'",
      'DONATE_DO_URL': process.env.DONATE_DO_URL ? `"${process.env.DONATE_DO_URL}"` : "'/nicememe'",
      'TWITCH_API_OAUTH_URL': process.env.TWITCH_API_OAUTH_URL ? `"${process.env.TWITCH_API_OAUTH_URL}"` : "'/nicememe'",
    }, (() => IS_PRODUCTION ? {
      // production-only global defines
    } : undefined)())),
    new webpack.optimize.DedupePlugin(),
    new webpack.optimize.OccurenceOrderPlugin(true),
    new webpack.optimize.AggressiveMergingPlugin(),
    new ExtractTextPlugin(IS_PRODUCTION ? 'css/[name].[contenthash].css' : 'css/[name].css'),
    // production-only plugins
    ...(() => IS_PRODUCTION ? [
      new webpack.optimize.UglifyJsPlugin({
        compress: {
          warnings: false,
        },
      }),
    ] : [])(),
  ].filter(Boolean),
};
