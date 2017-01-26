/* global process */

// define functions to run for services that have status checking here
const services = {
  'twitch': async stream => {
    let live = true;
    let thumbnail = null;

    // Twitch API v5: must convert channel name to numeric ID
    const twitchChannelIdResponse = await fetch(`https://api.twitch.tv/kraken/users?login=${stream.channel}`, {
      headers: {
        Accept: 'application/vnd.twitchtv.v5+json',
        'Client-ID': process.env.TWITCH_CLIENT_ID,
      },
    });
    const twitchChannelIdData = await twitchChannelIdResponse.json();
    if (!twitchChannelIdData.users || !twitchChannelIdData.users[0] || !twitchChannelIdData.users[0]._id) {
      throw new Error(`Unexpected twitch response from '/users': ${JSON.stringify(twitchChannelIdData)}`);
    }
    // TODO: figure out why Twitch returns an array of users
    const twitch_channel_id = twitchChannelIdData.users[0]._id;

    const twitchStreamResponse = await fetch(`https://api.twitch.tv/kraken/streams/${twitch_channel_id}`, {
      headers: {
        Accept: 'application/vnd.twitchtv.v5+json',
        'Client-ID': process.env.TWITCH_CLIENT_ID,
      },
    });
    const twitchStreamData = await twitchStreamResponse.json();

    // stream is offline, fallback to twitch's offline video banner
    if (!twitchStreamData.stream) {
      live = false;
      const twitchChannelResponse = await fetch(`https://api.twitch.tv/kraken/channels/${twitch_channel_id}`, {
        headers: {
          Accept: 'application/vnd.twitchtv.v5+json',
          'Client-ID': process.env.TWITCH_CLIENT_ID,
        },
      });
      const twitchChannelData = await twitchChannelResponse.json();
      if (twitchChannelData.video_banner) {
        thumbnail = twitchChannelData.video_banner;
      }
    }
    else {
      thumbnail = twitchStreamData.stream.preview.large;
    }
    // don't re-save if the data is the same
    if (stream.live !== live || stream.thumbnail !== thumbnail) {
      await stream.update({ live, thumbnail });
    }
    return stream;
  },
};

export default services;
