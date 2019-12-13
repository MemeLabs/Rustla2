// @flow

export type Poll = {|
  +id: string;
  +multi_vote: boolean;
  +options: {
    +[string]: number;
  };
  +subject: string;
  +loading?: boolean;
  +loaded?: boolean;
|};
