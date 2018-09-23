// @flow

/**
 * Checks whether or not `url` is a valid URL for an advanced stream. A valid
 * advanced stream URL is one which satisfies all of the following conditions:
 *
 *   1. The URL is a valid URL, as defined by the WHATWG URL Standard.
 *   2. The protocol of the URL is either "http:" or "https:".
 *
 * @param {URL} URLimpl Implementation of the `URL` class.
 * @param {string} url The URL to inspect.
 * @returns {boolean} Returns `true` if `url` is a valid URL for an advanced
 *  stream, else `false`.
 */
function isValidAdvancedUrl(URLimpl: Class<URL>, url: string): boolean {
  try {
    return ['http:', 'https:'].includes(new URLimpl(url).protocol);
  }
  // Catch the `TypeError` which indicates that `url` is not a valid URL.
  catch (e) {
    return false;
  }
}

/**
 * Returns a version of the `isValidAdvancedUrl` function which is bounded to
 * the given `URL` implementation.
 *
 * @param {URL} URLimpl Implementation of the `URL` class.
 * @returns {Function} Returns `isValidAdvancedUrl` bounded to `URLimpl`.
 */
function createIsValidAdvancedUrl(URLimpl: Class<URL>): Function {
  return isValidAdvancedUrl.bind(null, URLimpl);
}

// Not using `export default` here because the rest of the codebase will be
// `require()`ing this module instead of `import`ing it.
module.exports = createIsValidAdvancedUrl;
