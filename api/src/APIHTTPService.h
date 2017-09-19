#pragma once

#include <rapidjson/document.h>
#include <memory>

#include "DB.h"
#include "HTTPRequest.h"
#include "HTTPRouter.h"

namespace rustla2 {

class APIHTTPService {
 public:
  explicit APIHTTPService(std::shared_ptr<DB> db);

  void RegisterRoutes(HTTPRouter *router);

  void GetAPI(uWS::HttpResponse *res, HTTPRequest *req);

  void GetStreamer(uWS::HttpResponse *res, HTTPRequest *req);

  void GetProfile(uWS::HttpResponse *res, HTTPRequest *req);

  void PostProfile(uWS::HttpResponse *res, HTTPRequest *req);

 private:
  std::shared_ptr<DB> db_;
  rapidjson::Document profile_update_schema_;
};

}  // namespace rustla2
