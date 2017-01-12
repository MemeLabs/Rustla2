require('dotenv').config({ silent: true });

const webpack = require('webpack');
const path = require('path');
const ExtractTextPlugin = require('extract-text-webpack-plugin');

const { NODE_ENV } = process.env;
console.log(`Bundling for ${(NODE_ENV || 'development').toUpperCase()}`);


module.exports = {
  entry: {
    main: [
      path.resolve(__dirname, 'src/client.jsx')
    ],
  },
  output: {
    path: path.join(__dirname, './static'),
    filename: '[name].[chunkhash].js',
    publicPath: '/static',
    sourceMapFilename: '[name].[chunkhash].js.map',
    crossOriginLoading: 'anonymous',
  },
  devServer: {
    contentBase: path.join(__dirname, './lib'),
    host: '0.0.0.0',
    port: process.env.npm_package_config_dev_server_port || 3000,
    historyApiFallback: true,
    watchOptions: {
      aggregateTimeout: 100,
    },
  },
  debug: NODE_ENV !== 'production',
  cache: true,
  stats: {
    colors: true,
  },
  resolve: {
    extensions: ['', '.js', '.jsx', '.scss', '.json'],
  },
  module: {
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
            require.resolve('babel-preset-stage-0')
          ],
          cacheDirectory: true
        }
      },
      {
        test: /\.pug$/,
        loaders: [`pug?pretty=${NODE_ENV === 'development'}`],
      },
      {
        test: /\.json/,
        loaders: ['json'],
      },
      {
        test: /\.s?css$/,
        loader: ExtractTextPlugin.extract('style',
          'css?importLoaders=1!postcss!sass')
      },
      {
        test: /\.(eot|woff|woff2|ttf|png|jpg|svg|ttf|gif)/,
        loaders: ['url?limit=30000&name=[name]-[hash].[ext]'],
      },
    ],
  },
  postcss: [
    require('autoprefixer')({ browsers: ['last 2 versions'] }),
  ],
  plugins: [
    new (require('html-webpack-plugin'))({
      template: './src/index.ejs',
      filename: '../lib/index.html',
      chunksSortMode: 'none',
      inject: true,
      minify: {
        minifyCSS: true,
        minifyJS: true,
        collapseWhitespace: NODE_ENV === 'production',
      },
    }),
    new (require('webpack-subresource-integrity'))({
      hashFuncNames: ['sha256', 'sha384'],
      enabled: NODE_ENV === 'production',
      enabled: true,
    }),
    new webpack.DefinePlugin(Object.assign({
      'process.env.NODE_ENV': `"${NODE_ENV}"`,
      'GITHUB_URL': process.env.GITHUB_URL ? `"${process.env.GITHUB_URL}"` : "'https://github.com/ILiedAboutCake/Rustla2'",
      'DONATE_PAYPAL_URL': process.env.DONATE_PAYPAL_URL ? `"${process.env.DONATE_PAYPAL_URL}"` : "'/nicememe'",
      'DONATE_LINODE_URL': process.env.DONATE_LINODE_URL ? `"${process.env.DONATE_LINODE_URL}"` : "'/nicememe'",
      'DONATE_DO_URL': process.env.DONATE_DO_URL ? `"${process.env.DONATE_DO_URL}"` : "'/nicememe'",
      'TWITCH_API_OAUTH_URL': process.env.TWITCH_API_OAUTH_URL ? `"${process.env.TWITCH_API_OAUTH_URL}"` : "'/nicememe'",
    }, (() => NODE_ENV === 'production' ? {
      // production-only global defines
    } : undefined)())),
    new webpack.optimize.CommonsChunkPlugin({
      name: 'vendor',
    }),
    new webpack.optimize.DedupePlugin(),
    new webpack.optimize.OccurenceOrderPlugin(true),
    new webpack.optimize.AggressiveMergingPlugin(),

    new ExtractTextPlugin('css/[name].[contenthash].css'),

    // production-only plugins
    ...(() => NODE_ENV === 'production' ? [
      new webpack.optimize.UglifyJsPlugin(),
    ] : [])(),
  ].filter(Boolean),
};
