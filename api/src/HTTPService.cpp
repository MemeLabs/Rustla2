#include "HTTPService.h"

#include <folly/String.h>
#include <glog/logging.h>
#include <chrono>

#include "Config.h"
#include "HTTPResponseWriter.h"

namespace rustla2 {

HTTPService::HTTPService(std::shared_ptr<DB> db, uWS::Hub *hub)
    : db_(db),
      admin_service_(db_),
      api_service_(db_),
      auth_service_(db_),
      static_service_(Config::Get().GetPublicPath()) {
  admin_service_.RegisterRoutes(&router_);
  api_service_.RegisterRoutes(&router_);
  auth_service_.RegisterRoutes(&router_);
  static_service_.RegisterRoutes(&router_);

  hub->onHttpRequest([&](uWS::HttpResponse *res, uWS::HttpRequest uws_req,
                         char *data, size_t length, size_t remaining_bytes) {
    HTTPRequest req(uws_req);

    if (RejectBannedIP(res, &req)) {
      return;
    }

    if (!router_.Dispatch(res, &req)) {
      static_service_.ServeIndex(res);
      return;
    }
    req.WritePostData(data, length, remaining_bytes);

    if (remaining_bytes != 0) {
      res->setUserData(new HTTPRequest(std::move(req)));
    }
  });

  hub->onHttpData([&](uWS::HttpResponse *res, char *data, size_t length,
                      size_t remaining_bytes) {
    auto req = LoadHTTPRequestFromUserData(res);
    if (req != nullptr) {
      req->WritePostData(data, length, remaining_bytes);

      if (remaining_bytes == 0) {
        delete req;
        res->setUserData(nullptr);
      }
    }
  });

  hub->onCancelledHttpRequest([&](uWS::HttpResponse *res) {
    auto req = LoadHTTPRequestFromUserData(res);
    if (req != nullptr) {
      delete req;
      res->setUserData(nullptr);
    }
  });
}

bool HTTPService::RejectBannedIP(uWS::HttpResponse *res, HTTPRequest *req) {
  if (db_->GetBannedIPs()->Contains(req->GetClientIPHeader())) {
    HTTPResponseWriter writer(res);
    writer.Status(403, "Forbidden");
    writer.Body();
    return true;
  }

  return false;
}

}  // namespace rustla2
