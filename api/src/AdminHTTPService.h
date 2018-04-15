#pragma once

#include <rapidjson/document.h>
#include <memory>

#include "DB.h"
#include "HTTPRequest.h"
#include "HTTPRouter.h"

namespace rustla2 {

class AdminHTTPService {
 public:
  explicit AdminHTTPService(std::shared_ptr<DB> db);

  void RegisterRoutes(HTTPRouter *router);

  bool RejectNonAdmin(uWS::HttpResponse *res, HTTPRequest *req);

  void PostUsername(uWS::HttpResponse *res, HTTPRequest *req);

 private:
  std::shared_ptr<DB> db_;
  rapidjson::Document username_update_schema_;
};

}  // namespace rustla2
