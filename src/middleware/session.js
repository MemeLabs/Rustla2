/* global process */
import Cookies from 'cookies';
import isLocal from 'is-local-ip';
import jwt from 'jwt-simple';


const JWT_NAME = process.env.JWT_NAME || 'jwt';
let JWT_SECRET = process.env.JWT_SECRET;
if (!JWT_SECRET) {
  console.warn('Unsafely using default JWT secret');
  JWT_SECRET = 'PepoThink';
}


const getCookie = (req, res) => {
  const cookies = new Cookies(req, res);
  const cookie = cookies.get(JWT_NAME);
  return cookie;
};

const setCookie = (req, res, value) => {
  const cookies = new Cookies(req, res);
  cookies.set(JWT_NAME, value, {
    domain: req.hostname === 'localhost' || isLocal(req.hostname)
      ? ''
      : `.${req.hostname}`,
    expires: new Date(Date.now() + 1000 * 60 * 60 * 24 * 7), // 1 week
    httpOnly: false,
  });
};

export const setSession = (req, res, id) => {
  const token = jwt.encode({ id }, JWT_SECRET);
  setCookie(req, res, token);
};

const sessionMiddleware = (req, res, next) => {
  const cookie = getCookie(req, res);
  if (!cookie) {
    return next();
  }
  try {
    const session = jwt.decode(cookie, JWT_SECRET);
    req.session = session;
    // roll cookie
    setCookie(req, res, cookie);
  }
  catch (err) {
    console.error('[sessionMiddleware] ', err);
    res.clearCookie(JWT_NAME);
  }
  next();
};

export default sessionMiddleware;
