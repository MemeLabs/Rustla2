/**
 * Checks if a given service is a vod or a live stream
 * @param {string} service Name of the service e.g. twitch, twitch-vod,  youtube, etc.
 * @returns {boolean} Returns `true` if `service` is a video on demand (vod) service
 * else it returns `false`
 */
const isVod = (service) => {
  switch (service) {
    case 'twitch-vod':
    case 'youtube-playlist':
      return true;
    default:
      return false;
  }
};

export default isVod;
