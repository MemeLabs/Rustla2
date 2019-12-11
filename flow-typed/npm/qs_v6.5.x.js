// flow-typed signature: 5e77859111a239aa8570ebf1f7415e5f
// flow-typed version: 5a7f14d011/qs_v6.5.x/flow_>=v0.104.x

declare module "qs" {
  declare type ParseOptions = {
    allowPrototypes?: boolean,
    arrayLimit?: number,
    decoder?: Function,
    delimiter?: string,
    depth?: number,
    parameterLimit?: number,
    plainmixeds?: boolean,
    strictNullHandling?: boolean,
    ignoreQueryPrefix?: boolean,
    parseArrays?: boolean,
    allowDots?: boolean,
    ...
  };

  declare type ArrayFormat = "brackets" | "indices" | "repeat";

  declare type FilterFunction = (prefix: string, value: mixed) => mixed;
  declare type FilterArray = Array<string | number>;
  declare type Filter = FilterArray | FilterFunction;

  declare type StringifyOptions = {
    encoder?: Function,
    delimiter?: string,
    strictNullHandling?: boolean,
    skipNulls?: boolean,
    encode?: boolean,
    sort?: Function,
    allowDots?: boolean,
    serializeDate?: Function,
    encodeValuesOnly?: boolean,
    format?: string,
    addQueryPrefix?: boolean,
    arrayFormat?: ArrayFormat,
    filter?: Filter,
    ...
  };

  declare type Formatter = (mixed) => string;

  declare type Formats = {
    RFC1738: string,
    RFC3986: string,
    "default": string,
    formatters: {
      RFC1738: Formatter,
      RFC3986: Formatter,
      ...
    },
    ...
  };

  declare module.exports: {
    parse(str: string, opts?: ParseOptions): mixed,
    stringify(obj: {[string]: mixed, ...} | Array<mixed>, opts?: StringifyOptions): string,
    formats: Formats,
    ...
  };
}
