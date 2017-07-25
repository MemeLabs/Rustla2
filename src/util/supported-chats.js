/* global process */

const supportedChats = {
  'azubu': channel => `https://www.azubu.tv/${channel}/chatpopup`,
  'hitbox': channel => `https://www.hitbox.tv/embedchat/${channel}`,
  'twitch-vod': channel => `https://www.twitch.tv/${channel}/chat?popout=`,
  'twitch': channel => `https://www.twitch.tv/${channel}/chat?popout=`,
  'ustream': channel => `https://www.ustream.tv/socialstream/${channel}`,
  'vaughn': channel => `https://vaughnlive.tv/popout/chat/${channel}`,
  'youtube': channel => `https://gaming.youtube.com/live_chat?v=${channel}&output=embed&embed_domain=${process.env.NODE_ENV === 'production' ? 'overrustle.com' : 'localhost'}`,
};

export default supportedChats;
export const supportedChatServices = new Set(Object.keys(supportedChats));
