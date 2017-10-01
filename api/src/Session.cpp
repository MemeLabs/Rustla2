#include "Session.h"

#include <jwt/jwt_all.h>
#include <ctime>
#include <memory>

#include "Config.h"

namespace rustla2 {

std::string EncodeSessionCookie(const std::string& id) {
  const time_t eol = time(nullptr) + Config::Get().GetJWTTTL();
  std::unique_ptr<json_t, json_ptr_delete> json(
      json_pack("{ss, si}", "id", id.c_str(), "exp", eol));

  HS256Validator signer(Config::Get().GetJWTSecret());
  return JWT::Encode(&signer, json.get());
}

std::string DecodeSessionCookie(const std::string& cookie) {
  if (cookie == "") {
    return "";
  }

  std::unique_ptr<JWT> token;
  ExpValidator exp;
  HS256Validator signer(Config::Get().GetJWTSecret());
  try {
    token.reset(JWT::Decode(cookie, &signer, &exp));
  } catch (InvalidTokenError& e) {
    return "";
  }

  json_t* id = json_object_get(token->payload(), "id");
  if (id == nullptr || !json_is_string(id)) {
    return "";
  }

  return json_string_value(id);
}

}  // namespace rustla2
