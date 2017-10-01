#include "TwitchClient.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

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
  return std::string(GetData()["error"].GetString(),
                     GetData()["error"].GetStringLength());
}

std::string ErrorResult::GetMessage() const {
  return std::string(GetData()["message"].GetString(),
                     GetData()["message"].GetStringLength());
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
  return std::string(GetData()["name"].GetString(),
                     GetData()["name"].GetStringLength());
}

uint64_t UsersResult::User::GetID() const {
  const std::string id(data_["_id"].GetString(),
                       data_["_id"].GetStringLength());
  return std::stoull(id);
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
  return std::string(GetData()["access_token"].GetString(),
                     GetData()["access_token"].GetStringLength());
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
  const auto& preview = GetData()["stream"]["preview"]["large"];
  return std::string(preview.GetString(), preview.GetStringLength());
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
  return uri.IsNull() ? ""
                      : std::string(uri.GetString(), uri.GetStringLength());
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
  const auto& preview = GetData()["preview"]["large"];
  return std::string(preview.GetString(), preview.GetStringLength());
}

uint64_t VideosResult::GetViews() const {
  return GetData()["views"].GetUint64();
}

Client::Client(ClientConfig config) : config_(config) {}

APIStatus Client::GetOAuthToken(const std::string& code,
                                AuthTokenResult* result) {
  rapidjson::StringBuffer json;
  rapidjson::Writer<rapidjson::StringBuffer> writer(json);
  writer.StartObject();
  writer.Key("client_id");
  writer.String(config_.client_id.c_str());
  writer.Key("client_secret");
  writer.String(config_.client_secret.c_str());
  writer.Key("redirect_uri");
  writer.String(config_.redirect_uri.c_str());
  writer.Key("grant_type");
  writer.String("authorization_code");
  writer.Key("code");
  writer.String(code.c_str());
  writer.EndObject();

  CurlRequest req(GetKrakenURL("oauth2/token"));
  req.AddHeader("Content-Type: application/json");
  req.SetPostData(json.GetString(), json.GetSize());
  req.Submit();

  return LoadResultFromCurlRequest(req, result);
}

APIStatus Client::GetUserByOAuthToken(const std::string& token,
                                      UserResult* result) {
  CurlRequest req(GetKrakenURL("user"));
  req.AddHeader("Authorization: OAuth " + token);
  req.AddHeader("Client-ID: " + config_.client_id);
  req.AddHeader("Accept: application/vnd.twitchtv.v5+json");
  req.Submit();

  return LoadResultFromCurlRequest(req, result);
}

APIStatus Client::GetUsersByName(const std::string& name, UsersResult* result) {
  auto url = GetKrakenURL("users?login=" + name);
  return LoadResultFromURL(url, result);
}

APIStatus Client::GetStreamByID(const uint64_t channel_id,
                                StreamsResult* result) {
  auto url = GetKrakenURL("streams/" + std::to_string(channel_id));
  return LoadResultFromURL(url, result);
}

APIStatus Client::GetChannelByID(const uint64_t channel_id,
                                 ChannelsResult* result) {
  auto url = GetKrakenURL("channels/" + std::to_string(channel_id));
  return LoadResultFromURL(url, result);
}

APIStatus Client::GetVideosByID(const std::string& video_id,
                                VideosResult* result) {
  auto url = GetKrakenURL("videos/" + video_id);
  return LoadResultFromURL(url, result);
}

}  // namespace twitch
}  // namespace rustla2
