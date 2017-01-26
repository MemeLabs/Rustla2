/* global __dirname */
import cluster from 'cluster';
import os from 'os';
import cp from 'child_process';


const debug = require('debug')('overrustle:master');

if (cluster.isMaster) {

  // spawn our servers
  os.cpus().forEach(() => {
    cluster.fork();
  });
  ['disconnect', 'exit', 'error', 'listening', 'message', 'online'].map(evt => {
    cluster.on(evt, worker => debug(`worker ${worker.id} ${evt}`));
  });
  cluster.on('message', (worker, message) => {
    if (message && message.type === 'forward') {
      for (const id in cluster.workers) {
        const worker = cluster.workers[id];
        worker.send(message.payload);
      }
    }
  });

  // spawn livechecking server
  cp.fork(`${__dirname}/server-livecheck`);
}
else {
  require('./server-slave');
}
