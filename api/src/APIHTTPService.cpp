#include "APIHTTPService.h"

#include <rapidjson/schema.h>

#include "Config.h"
#include "HTTPResponseWriter.h"

namespace rustla2 {

APIHTTPService::APIHTTPService(std::shared_ptr<DB> db) : db_(db) {
  profile_update_schema_.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "username": {"type": "string"},
          "service": {"type": "string"},
          "channel": {"type": "string"},
          "left_chat": {"type": "boolean"}
        },
        "required": ["service", "channel"]
      }
    )json");
}

void APIHTTPService::RegisterRoutes(HTTPRouter *router) {
  const auto &api = Config::Get().GetAPI();

  router->Get(api, &APIHTTPService::GetAPI, this);
  router->Get(api + "/streamer/*", &APIHTTPService::GetStreamer, this);
  router->Get(api + "/profile", &APIHTTPService::GetProfile, this);
  router->Post(api + "/profile", &APIHTTPService::PostProfile, this);
}

void APIHTTPService::GetAPI(uWS::HttpResponse *res, HTTPRequest *req) {
  HTTPResponseWriter writer(res);
  writer.Status(200, "OK");
  writer.JSON(db_->GetStreams()->GetAPIJSON());
}

void APIHTTPService::GetStreamer(uWS::HttpResponse *res, HTTPRequest *req) {
  HTTPResponseWriter writer(res);

  auto user = db_->GetUsers()->GetByName(req->GetPathPart(3).toString());
  if (user == nullptr) {
    writer.Status(404, "Not Found");
    writer.JSON("{\"error\": \"invalid user\"}");
  } else {
    writer.Status(200, "OK");
    writer.JSON(user->GetStreamJSON());
  }
}

void APIHTTPService::GetProfile(uWS::HttpResponse *res, HTTPRequest *req) {
  HTTPResponseWriter writer(res);

  const auto name = req->GetSessionID();
  auto user = db_->GetUsers()->GetByName(name);
  if (name == "" || user == nullptr) {
    writer.Status(401, "Unauthorized");
    writer.JSON("{\"error\": \"unauthorized\"}");
  } else {
    writer.Status(200, "OK");
    writer.JSON(user->GetProfileJSON());
  }
}

void APIHTTPService::PostProfile(uWS::HttpResponse *res, HTTPRequest *req) {
  const auto name = req->GetSessionID();
  auto user = name == "" ? nullptr : db_->GetUsers()->GetByName(name);

  if (user == nullptr) {
    HTTPResponseWriter writer(res);
    writer.Status(401, "Unauthorized");
    writer.JSON("{\"error\": \"unauthorized\"}");
    return;
  }

  req->OnPostData([=](const char *data, const size_t length) {
    HTTPResponseWriter writer(res);

    rapidjson::SchemaDocument profile_update_schema(profile_update_schema_);
    rapidjson::SchemaValidator validator(profile_update_schema);
    rapidjson::Document json;
    json.Parse(data, length);

    if (json.HasParseError() || !json.Accept(validator)) {
      writer.Status(400, "Invalid Request");
      writer.JSON("{\"error\": \"malformed or invalid json\"}");
      return;
    }

    const std::string channel(json["channel"].GetString(),
                              json["channel"].GetStringLength());
    const std::string service(json["service"].GetString(),
                              json["service"].GetStringLength());
    if (!user->SetChannelAndService(channel, service)) {
      writer.Status(400, "Invalid Request");
      writer.JSON("{\"error\": \"invalid channel or service\"}");
      return;
    }

    if (json.HasMember("left_chat")) {
      user->SetLeftChat(json["left_chat"].GetBool());
    }

    if (!user->Save()) {
      writer.Status(500, "Internal Error");
      writer.JSON("{\"error\": \"error saving changes\"}");
      return;
    }

    writer.Status(200, "OK");
    writer.JSON(user->GetProfileJSON());
  });
}

}  // namespace rustla2
