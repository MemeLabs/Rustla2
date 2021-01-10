// @flow
/* global process */

const supportedChats: {
  [key: string]: (channel: string) => string
} = {
  'twitch-vod': channel => `https://www.twitch.tv/embed/${channel}/chat?darkpopout&parent=${location.host}`,
  'twitch': channel => `https://www.twitch.tv/embed/${channel}/chat?darkpopout&parent=${location.host}`,
  'youtube': channel => `https://youtube.com/live_chat?v=${channel}&embed_domain=${process.env.NODE_ENV === 'production' ? 'strims.gg' : 'localhost'}`,
};

export default supportedChats;
export const supportedChatServices = new Set<string>(Object.keys(supportedChats));
