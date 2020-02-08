# strims.gg

Livestream viewing with `strims.gg` chat.

## Setup

First, ensure that you have Node.js (version 10 or greater) and `npm`
(preferably the latest stable release) installed. Then,

``` bash
$ git clone https://github.com/MemeLabs/Rustla2.git
$ cd Rustla2/
$ cp .env.example .env
```

Edit `.env` to change various environment variables. Most importantly,
`JWT_SECRET` should **not** be left blank. The following is recommended:

```bash
$ sed -i "s/JWT_SECRET=/JWT_SECRET=$(head -c 22 /dev/urandom | base64 | tr -dc A-Za-z0-9)/" .env
```

The command is a little different on macOS:

```bash
$ sed -i "" "s/JWT_SECRET=/JWT_SECRET=$(head -c 22 /dev/urandom | base64 | tr -dc A-Za-z0-9)/" .env
```

Install dependencies and build the frontend:

``` bash
$ npm ci
$ npm run build
```

Then, follow the instructions in `api/README.md` for how to start the backend
(which includes the API server and a web server for the frontend).

### Creating Twitch client

Retrieving thumbnails, viewer counts, and live statuses for Twitch streams
requires a registered Twitch client.

  1. Go to <https://dev.twitch.tv/console/apps/create>
  2. Name the application whatever you want. The important part is that the
     **Redirect URI** is set to `$API/oauth`. For example:

     ![](https://i.imgur.com/hy4ii2c.png)
  3. Edit `.env` to include your **Redirect URI**, **Client ID**, and **Client
     Secret**:

     ```
     TWITCH_CLIENT_ID=yourclientid
     TWITCH_CLIENT_SECRET=yourclientsecret
     TWITCH_REDIRECT_URI=http://localhost:3000/oauth
     ```

## UI dev environment setup

Install the latest stable version of Node.

```bash
$ git clone https://github.com/MemeLabs/Rustla2.git
$ cd Rustla2/
$ cp .env.example .env
```

Update `API_WS` in the config with the production WebSocket API URL.

```
API_WS=wss://strims.gg/ws
```

Install the dependencies and start the webpack dev server

```bash
$ npm ci
$ npm run dev-server
```

You can access the dev server from your browser at `http://localhost:3000`.

## Building for production

The process of building for development is essentially the same as building for
production, except with the additional step of minifying the frontend JavaScript
code. This reduces the overall size of the bundle that is served to users, which
can result in faster page loads.

``` bash
npm run build:production
```
