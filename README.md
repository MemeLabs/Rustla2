# overrustle.com

Livestream viewing with `destiny.gg` chat.

## Setup

First, ensure that you have Node.js (version 4 or greater) and `npm` (preferably
the latest stable release) installed. Then,

``` bash
$ git clone https://github.com/ILiedAboutCake/Rustla2.git
$ cd Rustla2/
$ cp .env.example .env
```

Edit `.env` to change various environment variables.

Install dependencies, build assets, and run the server:

``` bash
$ npm install
$ npm run build
$ npm start
```

### Creating Twitch client

Retrieving thumbnails, viewer counts, and live statuses for Twitch streams
requires a registered Twitch client.

  1. Go to <https://www.twitch.tv/settings/connections>
  2. Register a new developer application
  3. Name the application whatever you want. The important part is that the
     **Redirect URI** is set to `$API/oauth`. For example:

     ![](https://i.imgur.com/SqG6TNB.png)
  4. Edit `.env` to include your **Redirect URI**, **Client ID**, and **Client
     Secret**:

     ```
     TWITCH_CLIENT_ID=yourclientid
     TWITCH_CLIENT_SECRET=yourclientsecret
     TWITCH_REDIRECT_URI=http://localhost:3000/oauth
     ```
