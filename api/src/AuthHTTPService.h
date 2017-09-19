#pragma once

#include <memory>

#include "DB.h"
#include "HTTPRouter.h"
#include "TwitchClient.h"

namespace rustla2 {

class AuthHTTPService {
 public:
  explicit AuthHTTPService(std::shared_ptr<DB> db);

  void RegisterRoutes(HTTPRouter *router);

  void GetLogin(uWS::HttpResponse *res, HTTPRequest *req);

  void GetOAuth(uWS::HttpResponse *res, HTTPRequest *req);

 private:
  std::shared_ptr<DB> db_;
  std::unique_ptr<twitch::Client> twitch_client_;
};

}  // namespace rustla2
