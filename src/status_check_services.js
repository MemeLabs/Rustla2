/* global process */

import querystring from 'querystring';


// define functions to run for services that have status checking here
const services = {
  'angelthump': async stream => {
    const res = await fetch(`https://angelthump.com/api/${stream.channel}`);
    const data = await res.json();
    const { live, thumbnail, viewers } = data;

    // Only update database if data has changed.
    if (stream.live !== live
      || stream.thumbnail !== thumbnail
      || stream.viewers !== viewers) {
      await stream.update({ live, thumbnail, viewers });
    }

    return stream;
  },

  'twitch': async stream => {
    let live = true;
    let thumbnail = null;
    let viewers = 0;

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
      viewers = twitchStreamData.stream.viewers;
    }
    // don't re-save if the data is the same
    if (stream.live !== live
      || stream.thumbnail !== thumbnail
      || stream.viewers !== viewers) {
      await stream.update({ live, thumbnail, viewers });
    }
    return stream;
  },

  'youtube': async stream => {
    // 'liveStreamingDetails' gets concurrent viewer count for live streams.
    // 'snippet' gets live status and thumbnail.
    const query = {
      key: process.env.GOOGLE_PUBLIC_API_KEY,
      part: ['liveStreamingDetails', 'snippet'].toString(),
      id: stream.channel,
    };
    const res = await fetch(`https://www.googleapis.com/youtube/v3/videos?${querystring.stringify(query)}`);
    const data = await res.json();

    // If a video or livestream exists with the given ID, then we consider the
    // "stream" to be live.
    const live = Boolean(data.items.length);

    // Default viewers to 0, for regular YouTube videos.
    let viewers = 0;
    let thumbnail = null;

    if (live) {
      const video = data.items[0];

      // If this is a livestream, then get proper viewer count.
      if (video.snippet.liveBroadcastContent === 'live') {
        viewers = parseInt(video.liveStreamingDetails.concurrentViewers, 10);
      }

      thumbnail = video.snippet.thumbnails.medium.url;
    }

    // Save new information to database if something has changed.
    if (stream.live !== live
      || stream.viewers !== viewers
      || stream.thumbnail !== thumbnail) {
      await stream.update({ live, thumbnail, viewers });
    }

    return stream;
  },
};

export default services;
