#include "AdminHTTPService.h"

#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <sstream>

#include "Config.h"
#include "HTTPResponseWriter.h"
#include "JSON.h"

namespace rustla2 {

AdminHTTPService::AdminHTTPService(std::shared_ptr<DB> db, uWS::Hub *hub)
    : db_(db), hub_(hub) {
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
          "nsfw": {"type": "boolean"},
          "hidden": {"type": "boolean"},
          "afk": {"type": "boolean"},
          "promoted": {"type": "boolean"},
          "removed": {"type": "boolean"}
        }
      }
    )json");

  ban_viewer_ips_schema_.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "stream_id": {"type": "number"},
          "note": {"type": "string"}
        },
        "required": ["stream_id"]
      }
    )json");
}

void AdminHTTPService::RegisterRoutes(HTTPRouter *router) {
  const auto &api = Config::Get().GetAPI();

  router->Post(api + "/admin/profiles/*/username",
               &AdminHTTPService::PostUsername, this);

  router->Post(api + "/admin/streams/**", &AdminHTTPService::PostStream, this);

  router->Get(api + "/admin/viewer-states", &AdminHTTPService::GetViewerStates,
              this);

  router->Post(api + "/admin/ban-viewers", &AdminHTTPService::BanViewers, this);
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
  if (path.size() == 6) {
    // ex: /api/admin/streams/{service}/{channel}

    Status status;
    auto channel = Channel::Create(req->GetPathPart(5).toString(),
                                   req->GetPathPart(4).toString(), &status);

    if (!status.Ok()) {
      HTTPResponseWriter writer(res);
      writer.Status(400, "Invalid Request");
      writer.JSON(json::Serialize(status));
      return;
    }

    stream = db_->GetStreams()->GetByChannel(channel);
  } else if (path.size() == 5) {
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
    if (input.HasMember("afk")) {
      stream->SetAFK(input["afk"].GetBool());
    }
    if (input.HasMember("promoted")) {
      stream->SetPromoted(input["promoted"].GetBool());
    }
    if (input.HasMember("removed")) {
      stream->SetRemoved(input["removed"].GetBool());
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

class ViewerStateBroadcaster {
 public:
  ViewerStateBroadcaster(Timer *timer, uWS::HttpResponse *res,
                         std::shared_ptr<ViewerStates> states)
      : timer_(timer),
        res_(res),
        states_(states),
        observer_(states->CreateObserver()) {
    std::stringstream header;
    header << "HTTP/1.1 200 OK\r\n"
           << "Connection: close\r\n"
           << "\r\n";

    res->write(header.str().c_str(), size_t(header.tellp()));
  }

  void Broadcast() {
    UserState state;
    while (states_->GetNextUserState(observer_, &state)) {
      rapidjson::Writer<rapidjson::StringBuffer> writer(buf_);
      state.WriteJSON(&writer);
      buf_.Put('\n');
      res_->write(buf_.GetString(), buf_.GetSize());
      buf_.Clear();
    }
  }

  void Stop() {
    timer_->stop();
    delete timer_;
    delete this;
  }

 private:
  Timer *timer_;
  uWS::HttpResponse *res_;
  rapidjson::StringBuffer buf_;
  std::shared_ptr<ViewerStates> states_;
  std::shared_ptr<ViewerStateObserver> observer_;
};

void AdminHTTPService::GetViewerStates(uWS::HttpResponse *res,
                                       HTTPRequest *req) {
  if (RejectNonAdmin(res, req)) {
    return;
  }

  auto timer = new Timer(hub_->getLoop());
  auto states = db_->GetViewerStates();
  auto broadcaster = new ViewerStateBroadcaster(timer, res, states);
  timer->setData(broadcaster);
  timer->start(
      [=](Timer *timer) {
        static_cast<ViewerStateBroadcaster *>(timer->getData())->Broadcast();
      },
      0, 250);

  req->OnCancel([=]() { broadcaster->Stop(); });

  req->SetKeepAlive(true);
}

void AdminHTTPService::BanViewers(uWS::HttpResponse *res, HTTPRequest *req) {
  if (RejectNonAdmin(res, req)) {
    return;
  }

  req->OnPostData([=](const char *data, const size_t length) {
    HTTPResponseWriter writer(res);

    rapidjson::SchemaDocument ban_viewer_ips(ban_viewer_ips_schema_);
    rapidjson::SchemaValidator validator(ban_viewer_ips);
    rapidjson::Document input;
    input.Parse(data, length);

    if (input.HasParseError() || !input.Accept(validator)) {
      writer.Status(400, "Invalid Request");
      writer.JSON("{\"error\": \"malformed or invalid json\"}");
      return;
    }

    auto stream = db_->GetStreams()->GetByID(input["stream_id"].GetUint64());
    if (stream == nullptr) {
      writer.JSON("{\"error\": \"stream id not found\"}");
      return;
    }

    std::string note = "";
    if (input.HasMember("note")) {
      note = json::StringRef(input["note"]);
    }

    db_->GetBannedIPs()->Insert(stream->GetViewerIPs(), note);

    writer.Status(200, "OK");
    writer.JSON(json::Serialize(Status(StatusCode::OK, "done")));
    return;
  });
}

}  // namespace rustla2
