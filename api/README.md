## Setup

  1. Build the static assets and create a config (<https://github.com/ILiedAboutCake/Rustla2#setup>)
  2. Install Docker (<https://docs.docker.com/engine/installation/>)
  3. Build and run the Docker container
        ```
        $ git submodule update --init
        $ cd api
        $ docker build . -f Dockerfile.base -t rustla2-api-base
        $ docker build . -t rustla2-api
        $ docker run -d --name rustla2 --net=host -v ~/Rustla2:/Rustla2:rw -w /Rustla2 rustla2-api:latest
        ```
