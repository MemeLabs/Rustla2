// @flow

type Poll = {|
  +id: string;
  +loaded?: boolean;
  +loading?: boolean;
  +multi_vote: boolean;
  +options: {|
    +[string]: number;
  |};
  +subject: string;
|};

type Stream = {|
  +afk_rustlers: number,
  +afk: boolean,
  +channel: string,
  +id: number,
  +live: boolean,
  +nsfw: boolean,
  +overrustle_id: string,
  +promoted: boolean,
  +rustlers: number,
  +service: string,
  +thumbnail: string,
  +title: string,
  +viewers: number
|};

export type State = {|
  +isAfk: boolean,
  +isLoading: boolean,
  +polls: {|
    +[string]: Poll
  |},
  +self: {|
    +isLoggedIn: boolean,
    +profile: {|
      +err: null,
      +isFetching: boolean,
      +data: null,
    |},
  |},
  +stream: number | null,
  +streams: {|
    +[number | null]: Stream
  |},
  +ui: {|
    +chatHost: Symbol,
    +chatSize: number,
    +showChat: boolean,
    +showHeader: boolean,
    +showFooter: boolean,
  |}
|};
