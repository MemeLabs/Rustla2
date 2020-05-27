#pragma once

#include <rapidjson/document.h>

#include <memory>

#include "DB.h"
#include "HTTPRequest.h"
#include "HTTPRouter.h"

namespace rustla2 {

class AdminHTTPService {
 public:
  explicit AdminHTTPService(std::shared_ptr<DB> db, uWS::Hub *hub);

  void RegisterRoutes(HTTPRouter *router);

  bool RejectNonAdmin(uWS::HttpResponse *res, HTTPRequest *req);

  void PostUsername(uWS::HttpResponse *res, HTTPRequest *req);

  void PostStream(uWS::HttpResponse *res, HTTPRequest *req);

  void GetViewerStates(uWS::HttpResponse *res, HTTPRequest *req);

  void BanViewers(uWS::HttpResponse *res, HTTPRequest *req);

 private:
  std::shared_ptr<DB> db_;
  uWS::Hub *hub_;
  rapidjson::Document username_update_schema_;
  rapidjson::Document stream_update_schema_;
  rapidjson::Document ban_viewer_ips_schema_;
};

}  // namespace rustla2
