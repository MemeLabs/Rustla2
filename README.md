# strims.gg

Livestream viewing with `strims.gg` chat.

## Setup

First, ensure that you have Node.js (version 6 or greater) and `npm` (preferably
the latest stable release) installed. Then,

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

Install dependencies and build the frontend:

``` bash
$ npm install
$ npm run build
```

Then, follow the instructions in `api/README.md` for how to start the backend
(which includes the API server and a web server for the frontend).

### Creating Twitch client

Retrieving thumbnails, viewer counts, and live statuses for Twitch streams
requires a registered Twitch client.

  1. Go to <https://glass.twitch.tv/>
  2. Register a new developer application
  3. Name the application whatever you want. The important part is that the
     **Redirect URI** is set to `$API/oauth`. For example:

     ![](https://i.imgur.com/jNN3I4R.png)
  4. Edit `.env` to include your **Redirect URI**, **Client ID**, and **Client
     Secret**:

     ```
     TWITCH_CLIENT_ID=yourclientid
     TWITCH_CLIENT_SECRET=yourclientsecret
     TWITCH_REDIRECT_URI=http://localhost:3000/oauth
     ```

## Building for production

The process of building for development is essentially the same as building for
production, except with the additional step of minifying the frontend JavaScript
code. This reduces the overall size of the bundle that is served to users, which
can result in faster page loads.

``` bash
npm run build:production
```
