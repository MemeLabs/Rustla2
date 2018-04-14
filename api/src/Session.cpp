#include "Session.h"

#include <jwt/jwt_all.h>
#include <ctime>
#include <memory>

#include "Config.h"

namespace rustla2 {

std::string EncodeSessionCookie(const std::string& id) {
  const time_t eol = time(nullptr) + Config::Get().GetJWTTTL();
  nlohmann::json json = {{"id", id}, {"exp", eol}};

  HS256Validator signer(Config::Get().GetJWTSecret());
  return JWT::Encode(signer, json);
}

std::string DecodeSessionCookie(const std::string& cookie) {
  if (cookie.empty()) {
    return "";
  }

  nlohmann::json header, payload;
  ExpValidator exp;
  HS256Validator signer(Config::Get().GetJWTSecret());
  try {
    std::tie(header, payload) = JWT::Decode(cookie, &signer, &exp);
  } catch (InvalidTokenError& e) {
    return "";
  }

  auto id = payload.find("id");
  if (id == payload.end() || !id->is_string()) {
    return "";
  }

  return id->get<std::string>();
}

}  // namespace rustla2
