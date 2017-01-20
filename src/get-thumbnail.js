import fetch from 'isomorphic-fetch';
import process from 'process';

export default async stream => {
  let response, data;

  switch (stream.service) {
    case 'twitch':
      response =
        await fetch(`https://api.twitch.tv/kraken/streams/${stream.twitch_channel_id}`,
          {
            headers: {
              Accept: 'application/vnd.twitchtv.v5+json',
              'Client-ID': process.env.TWITCH_CLIENT_ID,
            },
          });
      data = await response.json();

      // stream is offline, fallback to twitch's offline video banner
      if (!data.stream) {
        response =
          await fetch(`https://api.twitch.tv/kraken/channels/${stream.twitch_channel_id}`,
            {
              headers: {
                Accept: 'application/vnd.twitchtv.v5+json',
                'Client-ID': process.env.TWITCH_CLIENT_ID,
              },
            });
        data = await response.json();
        return data.video_banner;
      }

      return data.stream.preview.large;
  }
};
