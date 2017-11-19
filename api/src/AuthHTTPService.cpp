#include "AuthHTTPService.h"

#include <sstream>
#include <string>

#include "Config.h"
#include "HTTPResponseWriter.h"

namespace rustla2 {

AuthHTTPService::AuthHTTPService(std::shared_ptr<DB> db) : db_(db) {
  twitch::ClientConfig twitch_config{
      .client_id = Config::Get().GetTwitchClientID(),
      .client_secret = Config::Get().GetTwitchClientSecret(),
      .redirect_uri = Config::Get().GetTwitchRedirectURL()};
  twitch_client_.reset(new twitch::Client(twitch_config));
}

void AuthHTTPService::RegisterRoutes(HTTPRouter *router) {
  router->Get("/login", &AuthHTTPService::GetLogin, this);
  router->Get("/oauth", &AuthHTTPService::GetOAuth, this);
}

void AuthHTTPService::GetLogin(uWS::HttpResponse *res, HTTPRequest *req) {
  std::stringstream url;
  url << "https://api.twitch.tv/kraken/oauth2/authorize"
      << "?response_type=code"
      << "&scope=user_read"
      << "&client_id=" << Config::Get().GetTwitchClientID()
      << "&redirect_uri=" << Config::Get().GetTwitchRedirectURL();

  HTTPResponseWriter writer(res);
  writer.Status(302, "Found");
  writer.Header("Location", url.str());
  writer.Body();
}

void AuthHTTPService::GetOAuth(uWS::HttpResponse *res, HTTPRequest *req) {
  HTTPResponseWriter writer(res);

  const auto query_params = req->GetQueryParams();
  auto code = query_params.find("code");
  auto error = query_params.find("error");

  if (error != query_params.end()) {
    if (error->second == "redirect_mismatch") {
      LOG(ERROR) << "Got \"redirect mismatch\" error from Twitch. Please "
                    "ensure that you have set the TWITCH_REDIRECT_URI "
                    "environment variable to the redirect URI defined in your "
                    "Twitch application.";
    }

    writer.Status(401, "Unauthorized");
    writer.Header("Content-Type", "text/plain");
    writer.Body(
        "Authentication could not be completed because the server is "
        "misconfigured");
    return;
  }

  if (code == query_params.end()) {
    writer.Status(401, "Unauthorized");
    writer.Header("Content-Type", "text/plain");
    writer.Body("Authentication failed without error");
    return;
  }

  twitch::AuthTokenResult twitch_token;
  if (!twitch_client_->GetOAuthToken(code->second, &twitch_token).Ok()) {
    writer.Status(401, "Unauthorized");
    writer.Header("Content-Type", "text/plain");
    writer.Body("Failed to get token from OAuth exchange code");
    return;
  }
  auto access_token = twitch_token.GetAccessToken();

  twitch::UserResult twitch_user;
  if (!twitch_client_->GetUserByOAuthToken(access_token, &twitch_user).Ok()) {
    writer.Status(503, "Service Unavailable");
    writer.Header("Content-Type", "text/plain");
    writer.Body("Twitch API failure while retrieving user");
    return;
  }
  auto name = twitch_user.GetName();

  auto ip = req->GetClientIPHeader().toString();

  auto user = db_->GetUsers()->GetByName(name);
  if (user == nullptr) {
    auto channel = Channel::Create(name, kTwitchService.toString());
    user = db_->GetUsers()->Emplace(name, channel, ip);
  } else {
    user->SetLastIP(ip);
    user->Save();
  }

  writer.Status(302, "Redirect");
  writer.SessionCookie(name);
  writer.Header("Location", "/");
  writer.Body();
}

}  // namespace rustla2
