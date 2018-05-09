//utility function used to determine if a given service is a vod or a live stream
const isVod = (service) => {
  switch (service){
    case 'twitch-vod':
    case 'youtube-playlist':
    case 'hitbox-vod':
      return true;
    default:
      return false;
  }
};

export default isVod;
