/* global process */

import { STATUS_CODES } from 'http';


const errors = {
  HTTPError: class HTTPError {
    constructor() {
      Error.captureStackTrace(this, this.constructor);
    }
  },
};

for (const code in STATUS_CODES) {
  const name = STATUS_CODES[code];
  const formalName = name.replace(/\s+/g, '');
  // hack to get dynamic function name
  const err = {
    [formalName]: class extends errors.HTTPError {
      constructor(message, details) {
        super(message);
        this.error = name;
        this.message = message || this.error;
        this.status = Number(code);
        this.details = details;
      }
      // get JSON.stringify(this) to show the stack
      toJSON() {
        if (process.env.NODE_ENV !== 'production') {
          return {
            ...this,
            message: this.message,
            stack: this.stack,
          };
        }
        return {
          ...this,
          message: this.message,
        };
      }
    },
  };
  errors[formalName] = err[formalName];
}

export default errors;
