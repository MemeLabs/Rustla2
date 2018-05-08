#pragma once

#include <boost/functional/hash.hpp>
#include <folly/String.h>
#include <uWS/uWS.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "HTTPRequest.h"

namespace rustla2 {

using PathParts = std::vector<folly::StringPiece>;
using PathPartsIter = PathParts::iterator;

using HTTPRouteHandler =
    std::function<void(uWS::HttpResponse *, HTTPRequest *)>;

struct HTTPRoute {
  HTTPRoute() : HTTPRoute("") {}

  explicit HTTPRoute(const std::string &path)
      : HTTPRoute(path, uWS::HttpMethod::METHOD_INVALID) {}

  HTTPRoute(const std::string &path, const uWS::HttpMethod method)
      : path(path),
        method(method),
        wildcard_path(path == "*"),
        wildcard_subpath(path == "**") {}

  const std::string path;
  const uWS::HttpMethod method;
  const bool wildcard_path = false;
  const bool wildcard_subpath = false;
};

struct HTTPRouteHash : public std::unary_function<HTTPRoute, std::size_t> {
  std::size_t operator()(const HTTPRoute &k) const {
    auto hash = std::hash<std::string>{}(k.path);
    boost::hash_combine(hash, std::hash<std::underlying_type<uWS::HttpMethod>::type>{}(k.method));
    return hash;
  }
};

struct HTTPRouteEqual
    : public std::binary_function<HTTPRoute, HTTPRoute, bool> {
  bool operator()(const HTTPRoute &a, const HTTPRoute &b) const {
    return a.path == b.path && a.method == b.method;
  }
};

using HTTPRouteMap =
    std::unordered_map<HTTPRoute, std::unique_ptr<class HTTPRouterLayer>,
                       HTTPRouteHash, HTTPRouteEqual>;

class HTTPRouterLayer {
 public:
  HTTPRouterLayer() {}

  explicit HTTPRouterLayer(HTTPRoute route) : route_(route) {}

  explicit HTTPRouterLayer(HTTPRoute route, HTTPRouteHandler handler)
      : route_(route), handler_(handler) {}

  void InsertHandler(PathPartsIter first, PathPartsIter last,
                     const uWS::HttpMethod method, HTTPRouteHandler handler) {
    auto part = first + 1;
    HTTPRouterLayer *layer = this;
    for (auto j = last - 1; part != j; ++part) {
      layer = layer->CreateLayer((*part).toString());
    }

    HTTPRoute key((*part).toString(), method);
    layer->layers_[key].reset(new HTTPRouterLayer(key, handler));
  }

  bool Handle(PathPartsIter current, PathPartsIter last,
              const uWS::HttpMethod method,
              std::function<void(HTTPRouteHandler)> callback) const {
    const auto next = current + 1;
    const bool is_last = next == last;
    const bool method_matches =
        !is_last || (is_last && route_.method == method);
    const bool path_matches = *current == route_.path || route_.wildcard_path ||
                              route_.wildcard_subpath;

    if (!path_matches || !method_matches) {
      return false;
    }

    if (handler_ != nullptr && (is_last || route_.wildcard_subpath)) {
      callback(handler_);
      return true;
    }

    if (!is_last) {
      for (const auto &i : layers_) {
        if (i.second->Handle(next, last, method, callback)) {
          return true;
        }
      }
    }

    return false;
  }

 private:
  HTTPRouterLayer *CreateLayer(const std::string &path) {
    HTTPRoute key(path);
    if (layers_.find(key) == layers_.end()) {
      layers_[key].reset(new HTTPRouterLayer(key));
    }
    return layers_[key].get();
  }

  HTTPRoute route_;
  const HTTPRouteHandler handler_ = 0;
  HTTPRouteMap layers_;
};

class HTTPRouter {
 public:
  bool Dispatch(uWS::HttpResponse *res, HTTPRequest *req) {
    return Dispatch(req->GetPath(), req->GetMethod(),
                    [&](HTTPRouteHandler handler) { handler(res, req); });
  }

  bool Dispatch(PathParts parts, const uWS::HttpMethod method,
                std::function<void(HTTPRouteHandler)> callback) {
    return layer_.Handle(parts.begin(), parts.end(), method, callback);
  }

  bool Dispatch(const std::string &path, const uWS::HttpMethod method,
                std::function<void(HTTPRouteHandler)> callback) {
    PathParts parts;
    folly::split('/', path, parts);
    return Dispatch(parts, method, callback);
  }

  void Get(const std::string &path, HTTPRouteHandler handler) {
    InsertHandler(path, uWS::HttpMethod::METHOD_GET, handler);
  }

  void Post(const std::string &path, HTTPRouteHandler handler) {
    InsertHandler(path, uWS::HttpMethod::METHOD_POST, handler);
  }

  void Head(const std::string &path, HTTPRouteHandler handler) {
    InsertHandler(path, uWS::HttpMethod::METHOD_HEAD, handler);
  }

  template <typename T, typename I>
  void Get(const std::string &path, const T handler, I *instance) {
    Get(path, CreateHandler(handler, instance));
  }

  template <typename T, typename I>
  void Post(const std::string &path, const T handler, I *instance) {
    Post(path, CreateHandler(handler, instance));
  }

  template <typename T, typename I>
  void Head(const std::string &path, const T handler, I *instance) {
    Head(path, CreateHandler(handler, instance));
  }

 private:
  template <typename T, typename I>
  const HTTPRouteHandler CreateHandler(const T handler, I *instance) {
    return [=](uWS::HttpResponse *res, HTTPRequest *req) {
      (instance->*handler)(res, req);
    };
  }

  void InsertHandler(const std::string &path, const uWS::HttpMethod method,
                     HTTPRouteHandler handler) {
    PathParts parts;
    folly::split('/', path, parts);
    layer_.InsertHandler(parts.begin(), parts.end(), method, handler);
  }

  HTTPRouterLayer layer_;
};

}  // namespace rustla2
