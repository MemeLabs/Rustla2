import fetch from 'isomorphic-fetch';
import process from 'process';

export default async stream => {
  switch (stream.service) {
    case 'twitch':
      const response =
        await fetch(`https://api.twitch.tv/kraken/streams/${stream.channel}`,
          {
            headers: {
              'Client-ID': process.env.TWITCH_CLIENT_ID
            }
          });
      const data = await response.json();
      return data.stream.preview.large;
  }
};
