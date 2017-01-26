/* global __dirname */
import cluster from 'cluster';
import os from 'os';
import cp from 'child_process';


const debug = require('debug')('overrustle:master');

if (cluster.isMaster) {

  // spawn our servers
  os.cpus().forEach(() => {
    const worker = cluster.fork();
    ['disconnect', 'error', 'exit', 'listening', 'message', 'online'].map(evt => {
      worker.on(evt, () => debug(`worker ${worker.id} ${evt}`));
    });
  });

  // spawn livechecking server
  cp.fork(`${__dirname}/server-livecheck`);
}
else {
  require('./server-slave');
}
