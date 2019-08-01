// @flow

if (!global.Promise) {
  global.Promise = require('bluebird');
}

if (!global.fetch) {
  global.fetch = require('isomorphic-fetch');
}
