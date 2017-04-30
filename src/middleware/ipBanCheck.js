import { BannedIP } from '../db';
import errors from '../http_errors';

const ipBanCheckMiddleware = async (req, res, next) => {
  try {
    let bannedIP = await BannedIP.findById(req.connection.remoteAddress);
    if (!bannedIP) {
      return next();
    }
    throw new errors.Forbidden('you\'re banned. go cuck around somewhere else.');
  }
  catch (err) {
    next(err);
  }
};

export default ipBanCheckMiddleware;
