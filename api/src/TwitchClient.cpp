#include "TwitchClient.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "JSON.h"

namespace rustla2 {
namespace twitch {

rapidjson::Document ErrorResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "error": {"type": "string"},
          "message": {"type": "string"}
        },
        "required": ["error", "message"]
      }
    )json");
  return schema;
}

std::string ErrorResult::GetError() const {
  return json::StringRef(GetData()["error"]);
}

std::string ErrorResult::GetMessage() const {
  return json::StringRef(GetData()["message"]);
}

rapidjson::Document UserResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "_id": {
            "type": "string",
            "pattern": "^[0-9]+$"
          },
          "name": {"type": "string"}
        },
        "required": ["_id", "name"]
      }
    )json");
  return schema;
}

uint64_t UserResult::GetID() const { return GetData()["_id"].GetUint64(); }

std::string UserResult::GetName() const {
  return json::StringRef(GetData()["name"]);
}

uint64_t UsersResult::User::GetID() const {
  return std::stoull(std::string(json::StringRef(data_["_id"])));
}

rapidjson::Document UsersResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "_total": {"type": "integer"},
          "users": {
            "type": "array",
            "items": {
              "type": "object",
              "properties": {
                "_id": {
                  "type": "string",
                  "pattern": "^[0-9]+$"
                },
                "name": {"type": "string"}
              },
              "required": ["_id", "name"]
            }
          }
        },
        "required": ["_total", "users"]
      }
    )json");
  return schema;
}

bool UsersResult::IsEmpty() const { return GetSize() == 0; }

size_t UsersResult::GetSize() const { return GetData()["_total"].GetUint64(); }

const UsersResult::User UsersResult::GetUser(const size_t index) const {
  const auto& users = GetData()["users"].GetArray();
  return User(users[index]);
}

rapidjson::Document AuthTokenResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
    {
      "type": "object",
      "properties": {
        "access_token": {"type": "string"}
      },
      "required": ["access_token"]
    }
  )json");
  return schema;
}

std::string AuthTokenResult::GetAccessToken() const {
  return json::StringRef(GetData()["access_token"]);
}

rapidjson::Document StreamsResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "stream": {
            "anyOf": [
              {
                "type": "object",
                "properties": {
                  "viewers": {"type": "integer"},
                  "preview": {
                    "type": "object",
                    "properties": {
                      "large": {
                        "type": "string",
                        "format": "uri"
                      }
                    },
                    "required": ["large"]
                  }
                },
                "required": ["viewers", "preview"]
              },
              {"type": "null"}
            ]
          }
        },
        "required": ["stream"]
      }
    )json");
  return schema;
}

bool StreamsResult::IsEmpty() const { return GetData()["stream"].IsNull(); }

uint64_t StreamsResult::GetViewers() const {
  return GetData()["stream"]["viewers"].GetUint64();
}

std::string StreamsResult::GetLargePreview() const {
  return json::StringRef(GetData()["stream"]["preview"]["large"]);
}

rapidjson::Document ChannelsResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "video_banner": {
            "anyOf": [
              {
                "type": "string",
                "format": "uri"
              },
              {"type": "null"}
            ]
          }
        },
        "required": ["video_banner"]
      }
    )json");
  return schema;
}

std::string ChannelsResult::GetVideoBanner() const {
  const auto& uri = GetData()["video_banner"];
  return uri.IsNull() ? "" : std::string(json::StringRef(uri));
}

rapidjson::Document VideosResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "views": {"type": "integer"},
          "preview": {
            "type": "object",
            "properties": {
              "large": {
                "type": "string",
                "format": "uri"
              }
            },
            "required": ["large"]
          }
        },
        "required": ["views", "preview"]
      }
    )json");
  return schema;
}

std::string VideosResult::GetLargePreview() const {
  return json::StringRef(GetData()["preview"]["large"]);
}

uint64_t VideosResult::GetViews() const {
  return GetData()["views"].GetUint64();
}

Client::Client(ClientConfig config) : config_(config) {}

Status Client::GetOAuthToken(const std::string& code, AuthTokenResult* result) {
  rapidjson::StringBuffer json;
  rapidjson::Writer<rapidjson::StringBuffer> writer(json);
  writer.StartObject();
  writer.Key("client_id");
  writer.String(config_.client_id);
  writer.Key("client_secret");
  writer.String(config_.client_secret);
  writer.Key("redirect_uri");
  writer.String(config_.redirect_uri);
  writer.Key("grant_type");
  writer.String("authorization_code");
  writer.Key("code");
  writer.String(code);
  writer.EndObject();

  CurlRequest req(GetKrakenURL("oauth2/token"));
  req.AddHeader("Content-Type: application/json");
  req.SetPostData(json.GetString(), json.GetSize());
  req.Submit();

  return LoadResultFromCurlRequest(req, result);
}

Status Client::GetUserByOAuthToken(const std::string& token,
                                   UserResult* result) {
  CurlRequest req(GetKrakenURL("user"));
  req.AddHeader("Authorization: OAuth " + token);
  req.AddHeader("Client-ID: " + config_.client_id);
  req.AddHeader("Accept: application/vnd.twitchtv.v5+json");
  req.Submit();

  return LoadResultFromCurlRequest(req, result);
}

Status Client::GetUsersByName(const std::string& name, UsersResult* result) {
  auto url = GetKrakenURL("users?login=" + name);
  return LoadResultFromURL(url, result);
}

Status Client::GetStreamByID(const uint64_t channel_id, StreamsResult* result) {
  auto url = GetKrakenURL("streams/" + std::to_string(channel_id));
  return LoadResultFromURL(url, result);
}

Status Client::GetChannelByID(const uint64_t channel_id,
                              ChannelsResult* result) {
  auto url = GetKrakenURL("channels/" + std::to_string(channel_id));
  return LoadResultFromURL(url, result);
}

Status Client::GetVideosByID(const std::string& video_id,
                             VideosResult* result) {
  auto url = GetKrakenURL("videos/" + video_id);
  return LoadResultFromURL(url, result);
}

}  // namespace twitch
}  // namespace rustla2
