/* global process */
import Sequelize from 'sequelize';


const debug = require('debug')('overrustle:database');
let { DB_DB, DB_PATH, NODE_ENV } = process.env;

DB_DB = DB_DB || 'overrustle';
DB_PATH = DB_PATH || './overrustle.sqlite';

debug(`Using database ${DB_DB}:${DB_PATH}`);

export const sequelize = new Sequelize(DB_DB, null, null, {
  dialect: 'sqlite',
  storage: DB_PATH || ':memory:',

  logging: NODE_ENV === 'production' ? false : debug,

  define: {
    paranoid: false,
    underscored: true,
  },
});

export const User = sequelize.define('user', {
  // Twitch username
  id: {
    type: Sequelize.STRING,
    primaryKey: true,
    unique: true,
    allowNull: false,
    validate: {
      notEmpty: true,
    },
  },
  service: {
    type: Sequelize.STRING,
    allowNull: false,
    validate: {
      notEmpty: true,
    },
  },
  channel: {
    type: Sequelize.STRING,
    allowNull: false,
    validate: {
      notEmpty: true,
    },
  },
  last_ip: {
    type: Sequelize.STRING,
    allowNull: false,
    validate: {
      notEmpty: true,
    },
  },
  last_seen: {
    type: Sequelize.DATE,
    allowNull: false,
    validate: {
      notEmpty: true,
    },
  },
  left_chat: {
    type: Sequelize.BOOLEAN,
    defaultValue: false,
  },
});

export const Stream = sequelize.define('stream', {
  id: {
    type: Sequelize.INTEGER,
    defaultValue: Sequelize.UUIDV4,
    primaryKey: true,
    unique: true,
    allowNull: false,
    validate: {
      notEmpty: true,
    },
  },
  channel: {
    type: Sequelize.STRING,
    allowNull: false,
    unique: 'channel_service',
    validate: {
      notEmpty: true,
    },
  },
  service: {
    type: Sequelize.STRING,
    allowNull: false,
    unique: 'channel_service',
    validate: {
      notEmpty: true,
    },
  },
  overrustle_id: {
    type: Sequelize.STRING,
    references: {
      model: 'users',
      key: 'id',
    },
    onUpdate: 'CASCADE',
    onDelete: 'SET NULL',
  },
  thumbnail: {
    type: Sequelize.STRING,
  },
  live: {
    type: Sequelize.BOOLEAN,
    defaultValue: false,
  },
  // special query for this, see classMethods
  // rustlers: {},
  viewers: { // amount of people the service reports is watching this stream
    type: Sequelize.INTEGER,
    defaultValue: 0,
  },
}, {
  classMethods: {
    async findRustlersFor(id) {
      const res = await sequelize.query('SELECT COUNT(`rustlers`.`stream_id`) AS `rustlers` FROM `streams` AS `stream` LEFT OUTER JOIN `rustlers` AS `rustlers` ON `stream`.`id` = `rustlers`.`stream_id` WHERE `stream`.`id` = ? GROUP BY `stream`.`id` LIMIT 1;', {
        replacements: [id],
      });

      // If the rustler count is undefined, then there are 0 rustlers watching
      // this stream.
      if (!res[0].length) {
        return 0;
      }

      const [[{ rustlers }]] = res;
      return rustlers;
    },
    async findAllWithRustlers() {
      const [ streams ] = await sequelize.query('SELECT `stream`.*, COUNT(`rustlers`.`stream_id`) AS `rustlers` FROM `streams` AS `stream` LEFT OUTER JOIN `rustlers` AS `rustlers` ON `stream`.`id` = `rustlers`.`stream_id` GROUP BY `stream`.`id`;');

      // Fix live status not being boolean for some reason.
      streams.forEach(stream => {
        stream.live = Boolean(stream.live);
      });

      return streams;
    },
  },
});

export const Rustler = sequelize.define('rustler', {
  id: {
    type: Sequelize.UUIDV4,
    defaultValue: Sequelize.UUIDV4,
    primaryKey: true,
    unique: true,
    allowNull: false,
    validate: {
      notEmpty: true,
    },
  },
  stream_id: { // the Stream that the rustler is watching
    type: Sequelize.INTEGER,
    references: {
      model: 'streams',
      key: 'id',
    },
    onUpdate: 'CASCADE',
    onDelete: 'SET NULL',
  },
});

User.sync();
Rustler.sync({ force: true });
Stream.sync({ force: true });

Rustler.belongsTo(Stream, { as: 'stream' });
Stream.hasMany(Rustler);

Stream.belongsTo(User, { as: 'overrustle' });
