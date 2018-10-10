// @flow
/* global process */

const supportedChats: { [key: string]: (channel: string) => string } = {
  'twitch-vod': channel => `https://www.twitch.tv/embed/${channel}/chat?darkpopout`,
  'twitch': channel => `https://www.twitch.tv/embed/${channel}/chat?darkpopout`,
  'ustream': channel => `https://www.ustream.tv/socialstream/${channel}`,
  'vaughn': channel => `https://vaughnlive.tv/popout/chat/${channel}`,
  'youtube': channel => `https://youtube.com/live_chat?v=${channel}&embed_domain=${process.env.NODE_ENV === 'production' ? 'strims.gg' : 'localhost'}`,
};

export default supportedChats;
export const supportedChatServices = new Set<string>(Object.keys(supportedChats));
