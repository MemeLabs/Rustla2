#include "AdminHTTPService.h"

#include <rapidjson/schema.h>

#include "Config.h"
#include "HTTPResponseWriter.h"
#include "JSON.h"

namespace rustla2 {

AdminHTTPService::AdminHTTPService(std::shared_ptr<DB> db) : db_(db) {
  username_update_schema_.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "username": {"type": "string"}
        },
        "required": ["username"]
      }
    )json");

  stream_update_schema_.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "nsfw": {"type": "bool"},
          "hidden": {"type": "bool"}
        }
      }
    )json");
}

void AdminHTTPService::RegisterRoutes(HTTPRouter *router) {
  const auto &api = Config::Get().GetAPI();

  router->Post(api + "/admin/profiles/*/username",
               &AdminHTTPService::PostUsername, this);

  router->Post(api + "/admin/streams/**", &AdminHTTPService::PostStream, this);
}

bool AdminHTTPService::RejectNonAdmin(uWS::HttpResponse *res,
                                      HTTPRequest *req) {
  const auto id = req->GetSessionID();
  auto user = db_->GetUsers()->GetByID(id);

  if (user == nullptr || !user->GetIsAdmin()) {
    HTTPResponseWriter writer(res);
    writer.Status(401, "Unauthorized");
    writer.JSON("{\"error\": \"unauthorized\"}");
    return true;
  }

  return false;
}

void AdminHTTPService::PostUsername(uWS::HttpResponse *res, HTTPRequest *req) {
  if (RejectNonAdmin(res, req)) {
    return;
  }

  auto id = req->GetPathPart(4).toString();

  auto user = db_->GetUsers()->GetByName(id);
  if (user == nullptr) {
    user = db_->GetUsers()->GetByID(id);
  }

  if (user == nullptr) {
    HTTPResponseWriter writer(res);
    writer.Status(404, "Not Found");
    writer.JSON("{\"error\": \"user does not exist\"}");
    return;
  }

  auto newUser = std::make_shared<User>(*user);

  req->OnPostData([=](const char *data, const size_t length) {
    HTTPResponseWriter writer(res);

    rapidjson::SchemaDocument username_update_schema(username_update_schema_);
    rapidjson::SchemaValidator validator(username_update_schema);
    rapidjson::Document input;
    input.Parse(data, length);

    if (input.HasParseError() || !input.Accept(validator)) {
      writer.Status(400, "Invalid Request");
      writer.JSON("{\"error\": \"malformed or invalid json\"}");
      return;
    }

    auto status = newUser->SetName(json::StringRef(input["username"]));

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

void AdminHTTPService::PostStream(uWS::HttpResponse *res, HTTPRequest *req) {
  if (RejectNonAdmin(res, req)) {
    return;
  }

  std::shared_ptr<Stream> stream;

  auto path = req->GetPath();
  if (path.size() == 5) {
    // ex: /api/admin/streams/{service}/{channel}

    auto channel = Channel::Create(req->GetPathPart(4).toString(),
                                   req->GetPathPart(5).toString());

    stream = db_->GetStreams()->GetByChannel(channel);
  } else if (path.size() == 4) {
    // ex: /api/admin/streams/{stream_path}

    auto stream_path = req->GetPathPart(4).toString();
    auto user = db_->GetUsers()->GetByStreamPath(stream_path);

    if (user != nullptr) {
      stream = db_->GetStreams()->GetByChannel(*user->GetChannel());
    }
  }

  if (stream == nullptr) {
    HTTPResponseWriter writer(res);
    writer.Status(404, "Not Found");
    writer.JSON("{\"error\": \"stream does not exist\"}");
    return;
  }

  req->OnPostData([=](const char *data, const size_t length) {
    HTTPResponseWriter writer(res);

    rapidjson::SchemaDocument stream_update_schema(stream_update_schema_);
    rapidjson::SchemaValidator validator(stream_update_schema);
    rapidjson::Document input;
    input.Parse(data, length);

    if (input.HasParseError() || !input.Accept(validator)) {
      writer.Status(400, "Invalid Request");
      writer.JSON("{\"error\": \"malformed or invalid json\"}");
      return;
    }

    if (input.HasMember("nsfw")) {
      stream->SetNSFW(input["nsfw"].GetBool());
    }
    if (input.HasMember("hidden")) {
      stream->SetHidden(input["hidden"].GetBool());
    }

    if (stream->Save()) {
      writer.Status(200, "OK");
      writer.JSON(json::Serialize(stream));
      return;
    }

    writer.Status(500, "Internal Error");
    writer.JSON("{\"error\": \"error saving changes\"}");
    return;
  });
}

}  // namespace rustla2
