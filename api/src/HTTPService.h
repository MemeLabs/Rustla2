#include <uWS/uWS.h>
#include <memory>

#include "APIHTTPService.h"
#include "AdminHTTPService.h"
#include "AuthHTTPService.h"
#include "DB.h"
#include "HTTPRequest.h"
#include "HTTPRouter.h"
#include "StaticHTTPService.h"

namespace rustla2 {

class HTTPService {
 public:
  HTTPService(std::shared_ptr<DB> db, uWS::Hub *hub);

 private:
  inline HTTPRequest *LoadHTTPRequestFromUserData(uWS::HttpResponse *res) {
    return res == nullptr ? nullptr
                          : static_cast<HTTPRequest *>(res->getUserData());
  }

  bool RejectBannedIP(uWS::HttpResponse *res, HTTPRequest *req);

  std::shared_ptr<DB> db_;
  HTTPRouter router_;
  AdminHTTPService admin_service_;
  APIHTTPService api_service_;
  AuthHTTPService auth_service_;
  StaticHTTPService static_service_;
};

}  // namespace rustla2
