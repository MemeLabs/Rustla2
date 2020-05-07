## Setup

  1. Build the static assets and create a config (<https://github.com/MemeLabs/Rustla2#setup>)
  2. Install Docker (<https://docs.docker.com/engine/installation/>)
  3. Build the Docker container
        ```
        $ git submodule update --init
        $ docker build . -f Dockerfile.base -t rustla2-api-base
        $ docker build . -t rustla2-api
        $ docker run -d --name rustla2 -p 8076:8076 -v ~/Rustla2:/Rustla2:rw -w /Rustla2 rustla2-api:latest
        ```
  4. Set permissions
        ```
        $ sudo chown 1000:1000 ~/Rustla2
        ```
The user inside the container is mapped to 1000:1000, when mounting ~/Rustla2 onto the container it needs the same permissions to be able to write to the directory.

        
  5. Start the container
        ```
        $ docker run -d --name rustla2 -p 8076:8076 -v ~/Rustla2:/Rustla2:rw -w /Rustla2 rustla2-api:latest
        ```
If the first build fails, check your CPU and RAM usage. The process requires a fair amount of resources. Try setting the environment variable ``JOBS=1`` as explained in ``deps.sh`` and restart the process.

  4. Test if everything worked by running
        ```
        $ curl -v http://localhost:8076/api
        ```

### Manual account setup

It might be desirable for administrators or developers to create user accounts
without the need to go through the Twitch OAuth setup.

  1. Create a new account in the database (the database file will be created
     after first running the server):
        ```
        $ sqlite3 ./overrustle.sqlite
        sqlite> INSERT INTO "users" VALUES('13374242-1337-1337-1337-cccccccccccc',1337,'testuser','testuser','','twitch','admin','','2018-04-16 19:53:02',0,0,'','2018-04-01 20:00:00','2018-04-11 20:00:00',0,0,0);
        ```
  2. Forge the correct jwt cookie to be able to access this account:
        ```
        $ node
        > var jwt = require('jwt-simple');
        > jwt.encode({'id': '13374242-1337-1337-1337-cccccccccccc', 'exp':1999999999}, JWT_SECRET);
        ```

  3. In your browser create a cookie for your Rustla2 domain (possibly
        `localhost`) named `jwt` (or whatever you set `JWT_NAME` to be), and set
        the value of it to the output of the above. Depending on your browser,
        you might need to install some addon for this.

  4. Reload the page. You should be logged in, which can be seen at the top
        right of the page.

