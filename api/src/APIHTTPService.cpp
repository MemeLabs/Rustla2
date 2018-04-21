#include "APIHTTPService.h"

#include <rapidjson/schema.h>

#include "Config.h"
#include "HTTPResponseWriter.h"
#include "JSON.h"

namespace rustla2 {

APIHTTPService::APIHTTPService(std::shared_ptr<DB> db) : db_(db) {
  profile_update_schema_.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "username": {"type": "string"},
          "stream_path": {"type": "string"},
          "service": {"type": "string"},
          "channel": {"type": "string"},
          "left_chat": {"type": "boolean"}
        },
        "required": ["username", "stream_path", "service", "channel"]
      }
    )json");
}

void APIHTTPService::RegisterRoutes(HTTPRouter *router) {
  const auto &api = Config::Get().GetAPI();

  router->Get(api, &APIHTTPService::GetAPI, this);
  router->Get(api + "/streamer/*", &APIHTTPService::GetStreamer, this);
  router->Get(api + "/username/*", &APIHTTPService::GetUsername, this);
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

  auto user = db_->GetUsers()->GetByStreamPath(req->GetPathPart(3).toString());
  if (user == nullptr) {
    writer.Status(404, "Not Found");
    writer.JSON("{\"error\": \"invalid user\"}");
  } else {
    writer.Status(200, "OK");
    writer.JSON(user->GetStreamJSON());
  }
}

void APIHTTPService::GetUsername(uWS::HttpResponse *res, HTTPRequest *req) {
  HTTPResponseWriter writer(res);

  auto user = db_->GetUsers()->GetByID(req->GetPathPart(3).toString());
  if (user == nullptr) {
    writer.Status(404, "Not Found");
    writer.JSON("{\"error\": \"invalid user\"}");
  } else {
    writer.Status(200, "OK");
    writer.JSON(user->GetUsernameJSON());
  }
}

void APIHTTPService::GetProfile(uWS::HttpResponse *res, HTTPRequest *req) {
  HTTPResponseWriter writer(res);

  const auto id = req->GetSessionID();
  auto user = db_->GetUsers()->GetByID(id);
  if (id == "" || user == nullptr) {
    writer.Status(401, "Unauthorized");
    writer.JSON("{\"error\": \"unauthorized\"}");
  } else {
    writer.Status(200, "OK");
    writer.JSON(user->GetProfileJSON());
  }
}

void APIHTTPService::PostProfile(uWS::HttpResponse *res, HTTPRequest *req) {
  const auto id = req->GetSessionID();
  auto user = id == "" ? nullptr : db_->GetUsers()->GetByID(id);

  if (user == nullptr) {
    HTTPResponseWriter writer(res);
    writer.Status(401, "Unauthorized");
    writer.JSON("{\"error\": \"unauthorized\"}");
    return;
  }

  auto newUser = std::make_shared<User>(*user);

  req->OnPostData([=](const char *data, const size_t length) {
    HTTPResponseWriter writer(res);

    rapidjson::SchemaDocument profile_update_schema(profile_update_schema_);
    rapidjson::SchemaValidator validator(profile_update_schema);
    rapidjson::Document input;
    input.Parse(data, length);

    if (input.HasParseError() || !input.Accept(validator)) {
      writer.Status(400, "Invalid Request");
      writer.JSON("{\"error\": \"malformed or invalid json\"}");
      return;
    }

    Status status;

    if (input.HasMember("left_chat")) {
      newUser->SetLeftChat(input["left_chat"].GetBool());
    }

    auto name = json::StringRef(input["username"]);
    if (name.Empty()) {
      status =
          Status(StatusCode::VALIDATION_ERROR, "Username cannot be empty.");
    } else if (!newUser->GetName().empty() && name != newUser->GetName()) {
      status =
          Status(StatusCode::VALIDATION_ERROR, "Username cannot be changed.");
    } else {
      status = newUser->SetName(name);
    }

    if (status.Ok()) {
      newUser->SetChannel(Channel::Create(
          json::StringRef(input["channel"]), json::StringRef(input["service"]),
          json::StringRef(input["stream_path"]), &status));
    }

    if (status.Ok()) {
      status = db_->GetUsers()->Save(newUser);
    }

    if (status.Ok()) {
      writer.Status(200, "OK");
      writer.JSON(newUser->GetProfileJSON());
      return;
    }

    if (status.GetCode() == StatusCode::VALIDATION_ERROR) {
      writer.Status(400, "Invalid Request");
      writer.JSON(json::Serialize(status));
      return;
    }

    writer.Status(500, "Internal Error");
    writer.JSON("{\"error\": \"error saving changes\"}");
    return;
  });
}

}  // namespace rustla2
