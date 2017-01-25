/* global process */
require('dotenv').config();

import 'babel-polyfill';
import 'isomorphic-fetch';

import { Stream } from './db';
import services from './status_check_services';


const debug = require('debug')('overrustle:websocket');

// runs the functions defined above with the appropriate streams in the db/store
const service_names = Object.keys(services);
const updateStreamInfo = async () => {
  // get streams of which we have defined functions to run
  const streamsToUpdate = await Stream.findAll({
    where: {
      $or: service_names.map(service => ({ service })),
    },
  });
  // round em up
  const streamsByService = streamsToUpdate.reduce((acc, stream) => {
    let streams = acc.get(stream.service);
    if (!streams) {
      streams = [];
      acc.set(stream.service, streams);
    }
    streams.push(stream);
    return acc;
  }, new Map());
  // call the defined service functions above
  return Promise.all(Array.from(streamsByService.entries()).map(([ service, streams ]) => {
    debug(`fetching status of ${streams.length} ${service} streams`);
    return Promise.all(streams.map(async stream => {
      try {
        await services[service](stream);
        debug(`updated ${service}/${stream.channel}`);
      }
      catch (err) {
        debug(`Failed to update ${service}/${stream.channel}`, err);
      }
    }));
  }));
};

setInterval(updateStreamInfo, process.env.LIVECHECK_INTERVAL || 60 * 1000);
