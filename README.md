# overrustle.com

Livestream viewing with `destiny.gg` chat.

## Setup

First, ensure that you have Node.js 7 and `npm@4` installed. Then,

``` bash
$ git clone https://github.com/ILiedAboutCake/Rustla2.git
$ cd Rustla2/
$ touch .env
```

Populate `.env` with the the necessary environment variables:

```
PORT=3000
TWITCH_CLIENT_ID=abc123
```

Install dependencies, build assets, and run the server:

``` bash
$ npm install
$ npm run build
$ npm start
```
