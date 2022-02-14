#include "TwitchClient.h"

#include <folly/Format.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <string>

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

uint64_t UsersResult::User::GetID() const {
  return std::stoull(std::string(json::StringRef(data_["id"])));
}

std::string UsersResult::User::GetName() const {
  return json::StringRef(data_["id"]);
}

std::string UsersResult::User::GetOfflineImageURL() const {
  return json::StringRef(data_["offline_image_url"]);
}

rapidjson::Document UsersResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "data": {
            "type": "array",
            "minItems": "1",
            "items": {
              "type": "object",
              "properties": {
                "id": {
                  "type": "string",
                  "pattern": "^[0-9]+$"
                },
                "display_name": {"type": "string"},
                "offline_image_url": {
                  "anyOf": [
                    {
                      "type": "string",
                      "format": "uri"
                    },
                    {"type": "null"}
                  ]
                }
              },
              "required": ["id", "display_name", "offline_image_url"]
            }
          }
        },
        "required": ["data"]
      }
    )json");
  return schema;
}

bool UsersResult::IsEmpty() const { return GetSize() == 0; }

size_t UsersResult::GetSize() const {
  return GetData()["data"].GetArray().Size();
}

const UsersResult::User UsersResult::GetUser(const size_t index) const {
  const auto& users = GetData()["data"].GetArray();
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

uint64_t AuthTokenResult::GetExpiresIn() const {
  return GetData()["expires_in"].GetUint64();
}

rapidjson::Document StreamsResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "data": {
            "type": "array",
            "items": {
              "type": "object",
              "properties": {
                "game_name": {"type": "string"},
                "viewer_count": {"type": "integer"},
                "thumbnail_url": {
                  "type": "string",
                  "format": "uri"
                }
              },
              "required": ["game_name", "viewer_count", "thumbnail_url"]
            }
          }
        },
        "required": ["data"]
      }
    )json");
  return schema;
}

const rapidjson::Value& StreamsResult::GetStreamData() const {
  return GetData()["data"].GetArray()[0];
}

bool StreamsResult::IsEmpty() const {
  return GetData()["data"].GetArray().Size() == 0;
}

std::string StreamsResult::GetGame() const {
  return json::StringRef(GetStreamData()["game_name"]);
}

uint64_t StreamsResult::GetViewers() const {
  return GetStreamData()["viewer_count"].GetUint64();
}

std::string StreamsResult::GetThumbnailURL() const {
  std::string url = json::StringRef(GetStreamData()["thumbnail_url"]);
  url = url.replace(url.find("{height}"), 8, "360");
  url = url.replace(url.find("{width}"), 7, "640");
  return url;
}

rapidjson::Document VideosResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "data": {
            "type": "array",
            "minItems": "1",
            "items": {
              "type": "object",
              "properties": {
                "view_count": {"type": "integer"},
                "thumbnail_url": {
                  "type": "string",
                  "format": "uri"
                },
                "title": {"type": "string"}
              },
              "required": ["view_count", "thumbnail_url", "title"]
            }
          }
        },
        "required": ["data"]
      }
    )json");
  return schema;
}

const rapidjson::Value& VideosResult::GetVideoData() const {
  return GetData()["data"].GetArray()[0];
}

std::string VideosResult::GetTitle() const {
  return json::StringRef(GetVideoData()["title"]);
}

std::string VideosResult::GetThumbnailURL() const {
  std::string url = json::StringRef(GetVideoData()["thumbnail_url"]);
  url = url.replace(url.find("%{height}"), 9, "360");
  url = url.replace(url.find("%{width}"), 8, "640");
  return url;
}

uint64_t VideosResult::GetViews() const {
  return GetVideoData()["view_count"].GetUint64();
}

Client::Client(ClientConfig config) : config_(config) {}

Status Client::GetOAuthToken(const std::string& code, AuthTokenResult* result) {
  std::stringstream url;
  url << "https://id.twitch.tv/oauth2/token"
      << "?grant_type=authorization_code"
      << "&client_id=" << config_.client_id
      << "&client_secret=" << config_.client_secret
      << "&redirect_uri=" << config_.redirect_uri << "&code=" << code;

  CurlRequest req(url.str());
  req.Submit();

  return LoadResultFromCurlRequest(req, result);
}

Status Client::GetUserByOAuthToken(const std::string& token,
                                   UsersResult* result) {
  CurlRequest req(GetHelixURL("user"));
  req.AddHeader("Authorization: Bearer " + token);
  req.AddHeader("Client-ID: " + config_.client_id);
  req.SetPostData(nullptr, 0);
  req.Submit();

  return LoadResultFromCurlRequest(req, result);
}

Status Client::GetUsersByName(const std::string& name, UsersResult* result) {
  auto url = GetHelixURL("users?login=" + name);
  return LoadResultFromURL(url, result);
}

Status Client::GetStreamByID(const uint64_t channel_id, StreamsResult* result) {
  auto url = GetHelixURL("streams?user_id=" + std::to_string(channel_id));
  return LoadResultFromURL(url, result);
}

Status Client::GetVideosByID(const std::string& video_id,
                             VideosResult* result) {
  auto url = GetHelixURL("videos?id=" + video_id);
  return LoadResultFromURL(url, result);
}

Status Client::GetServerOAuthToken(AuthTokenResult* result) {
  std::stringstream url;
  url << "https://id.twitch.tv/oauth2/token"
      << "?grant_type=client_credentials"
      << "&client_id=" << config_.client_id
      << "&client_secret=" << config_.client_secret;

  CurlRequest req(url.str());
  req.SetPostData(nullptr, 0);
  req.Submit();

  return LoadResultFromCurlRequest(req, result);
}

std::string Client::GetAccessToken() {
  auto now = std::chrono::steady_clock::now();

  boost::lock_guard<boost::mutex> lock{access_token_lock_};

  if (now < access_token_eol_) {
    return access_token_;
  }

  AuthTokenResult token;
  auto token_status = GetServerOAuthToken(&token);
  if (!token_status.Ok()) {
    return "";
  }

  access_token_eol_ = now + std::chrono::seconds(token.GetExpiresIn());
  access_token_ = token.GetAccessToken();

  return access_token_;
}

}  // namespace twitch
}  // namespace rustla2
