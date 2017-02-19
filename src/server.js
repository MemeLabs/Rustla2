/* global __dirname */
import './bootstrap';
import cluster from 'cluster';
import os from 'os';
import cp from 'child_process';


const debug = require('debug')('overrustle:master');

if (cluster.isMaster) {

  // spawn our servers
  os.cpus().forEach(() => {
    cluster.fork();
  });
  // set up debug logging on worker events
  ['disconnect', 'exit', 'error', 'listening', 'message', 'online'].map(evt => {
    cluster.on(evt, worker => debug(`worker ${worker.id} ${evt}`));
  });
  // set up basic message relaying
  cluster.on('message', (worker, message) => {
    if (message && message.type === 'forward') {
      for (const id in cluster.workers) {
        const worker = cluster.workers[id];
        worker.send(message.payload);
      }
    }
  });

  // spawn livechecking server
  const livecheck = cp.fork(`${__dirname}/server-livecheck`);
  // set up messaging between livecheck and workers
  livecheck.on('message', message => {
    if (message && message.type === 'forward') {
      for (const id in cluster.workers) {
        const worker = cluster.workers[id];
        worker.send(message.payload);
      }
    }
  });
}
else {
  // start working
  require('./server-slave');
}
