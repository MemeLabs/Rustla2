// flow-typed signature: 5bec30cae35e00b8baa2fd59fc449fb2
// flow-typed version: 45acb9a3f7/isomorphic-fetch_v2.x.x/flow_>=v0.25.x

declare module "isomorphic-fetch" {
  declare module.exports: (
    input: string | Request | URL,
    init?: RequestOptions
  ) => Promise<Response>;
}
