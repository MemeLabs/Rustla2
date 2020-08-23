/* global process __dirname */
require('dotenv').config({ silent: true });

const webpack = require('webpack');
const path = require('path');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const gitHash = require('helper-git-hash');
const TerserPlugin = require('terser-webpack-plugin');
const LodashWebpackPlugin = require('lodash-webpack-plugin');
const HtmlWebpackExcludeAssetsPlugin = require('html-webpack-exclude-assets-plugin');
const HtmlWebpackPlugin = require('html-webpack-plugin');

const { NODE_ENV } = process.env;
const IS_PRODUCTION = NODE_ENV === 'production';
const IS_PRODUCTION_PROFILE = IS_PRODUCTION && process.argv.includes('--profile');

// eslint-disable-next-line no-console
console.log(`Bundling for ${(NODE_ENV || 'development').toUpperCase()}`);

module.exports = {
  mode: IS_PRODUCTION ? 'production' : 'development',
  devtool: !IS_PRODUCTION && 'eval-source-map',
  entry: {
    polyfills: path.resolve(__dirname, 'src/polyfills.js'),
    main: [
      '@babel/polyfill',
      path.resolve(__dirname, 'src/css/main.scss'),
      path.resolve(__dirname, 'src/client.jsx'),
    ]
  },
  output: {
    path: path.resolve(__dirname, './public/assets'),
    filename: IS_PRODUCTION ? '[name].[chunkhash].js' : '[name].js',
    chunkFilename: '[name].[chunkhash].js',
    publicPath: '/assets/',
    crossOriginLoading: 'anonymous',
  },
  optimization: {
    minimizer: [
      new TerserPlugin({
        terserOptions: {
          parse: { ecma: 8 },
          compress: {
            comparisons: false,
            ecma: 5,
            inline: 2,
            warnings: false
          },
          mangle: { safari10: true },
          keep_classnames: IS_PRODUCTION_PROFILE,
          keep_fnames: IS_PRODUCTION_PROFILE,
          output: {
            ascii_only: true,
            comments: false,
            ecma: 5
          }
        },
        sourceMap: true
      })
    ],
  },
  devServer: {
    contentBase: path.join(__dirname, './public'),
    host: '0.0.0.0',
    port: 3000,
    historyApiFallback: true,
    watchOptions: {
      aggregateTimeout: 100,
    },
    proxy: {
      '/api': {
        target: 'http://localhost:3001',
        secure: false,
      },
      '/scala-api': {
        target: 'http://localhost:3002',
        pathRewrite: {'^/scala-api' : ''},
        secure: false,
      },
    },
  },
  cache: true,
  stats: {
    children: false,
    entrypoints: false,
    modules: false,
  },
  resolve: {
    extensions: ['.js', '.jsx', '.scss', '.json'],
  },
  module: {
    rules: [
      {
        test: /\.jsx?$/,
        loader: 'eslint-loader',
        enforce: 'pre',
      },
      {
        test: /\.jsx?$/,
        include: path.resolve(__dirname, 'src'),
        loader: 'babel-loader',
        options: {
          cacheDirectory: true,
        },
      },
      {
        test: /\.s?css$/,
        use: [
          MiniCssExtractPlugin.loader,
          { loader: 'css-loader', options: { importLoaders: 1 } },
          {
            loader: 'postcss-loader',
            options: {
              ident: 'postcss',
              plugins: [require('autoprefixer')()]
            }
          },
          'sass-loader',
        ],
      },
      {
        test: /\.(eot|woff|woff2|ttf|png|jpg|svg|gif)/,
        use: ['url-loader?limit=30000&name=[name]-[hash].[ext]'],
      },
    ],
  },
  plugins: [
    new HtmlWebpackPlugin({
      excludeAssets: [/(main|polyfills)(\..*)?\.js$/],
      filename: '../index.html',
      inject: true,
      minify: IS_PRODUCTION && {
        collapseWhitespace: true,
        removeComments: true,
        removeRedundantAttributes: true,
        removeScriptTypeAttributes: true,
        removeStyleLinkTypeAttributes: true,
      },
      template: './src/index.ejs',
    }),
    new HtmlWebpackExcludeAssetsPlugin(),
    // new (require('webpack-subresource-integrity'))({
    //   hashFuncNames: ['sha256', 'sha384'],
    //   enabled: IS_PRODUCTION,
    // }),
    new webpack.DefinePlugin(
      Object.assign(
        {
          /* eslint-disable quotes */
          'process.env.NODE_ENV': `"${NODE_ENV}"`,
          API: process.env.API ? `"${process.env.API}"` : "'/api'",
          SCALA_API: process.env.SCALA_API ? `"${process.env.SCALA_API}"` : "'/scala-api'",
          API_WS: process.env.API_WS ? `"${process.env.API_WS}"` : undefined,
          JWT_NAME: process.env.JWT_NAME
            ? `"${process.env.JWT_NAME}"`
            : "'jwt'",
          GITHUB_URL: process.env.GITHUB_URL
            ? `"${process.env.GITHUB_URL}"`
            : "'https://github.com/MemeLabs/Rustla2'",
          DISCORD_URL: process.env.DISCORD_URL
            ? `"${process.env.DISCORD_URL}"`
            : "'https://discord.gg/qSXUukt'",
          DONATE_PAYPAL_URL: process.env.DONATE_PAYPAL_URL
            ? `"${process.env.DONATE_PAYPAL_URL}"`
            : undefined,
          DONATE_LINODE_URL: process.env.DONATE_LINODE_URL
            ? `"${process.env.DONATE_LINODE_URL}"`
            : undefined,
          DONATE_DO_URL: process.env.DONATE_DO_URL
            ? `"${process.env.DONATE_DO_URL}"`
            : undefined,
          CHAT_URL: process.env.CHAT_URL
            ? `"${process.env.CHAT_URL}"`
            : undefined,
          CHAT2_URL: process.env.CHAT2_URL
            ? `"${process.env.CHAT2_URL}"`
            : undefined,
          CHAT2_DOMAIN: process.env.CHAT2_DOMAIN
            ? `"${process.env.CHAT2_DOMAIN}"`
            : undefined,
          THUMBNAIL_REFRESH_INTERVAL: process.env.THUMBNAIL_REFRESH_INTERVAL
            ? `"${process.env.THUMBNAIL_REFRESH_INTERVAL}"`
            : undefined,
          GIT_COMMIT_HASH: `"${gitHash()}"`,
          GIT_SHORT_COMMIT_HASH: `"${gitHash({ short: true })}"`,
          AFK_TIMEOUT: process.env.AFK_TIMEOUT,
          /* eslint-enable quotes */
        },
        (() =>
          IS_PRODUCTION
            ? {
                // production-only global defines
            }
            : undefined)()
      )
    ),
    new webpack.optimize.AggressiveMergingPlugin(),
    new MiniCssExtractPlugin({
      filename: IS_PRODUCTION ? '[name].[contenthash].css' : '[name].css',
    }),
    new LodashWebpackPlugin(),
  ],
};
