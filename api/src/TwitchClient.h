#pragma once

#include <rapidjson/document.h>
#include <string>

#include "APIClient.h"
#include "Curl.h"
#include "Status.h"

namespace rustla2 {
namespace twitch {

class ErrorResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  std::string GetError() const;

  std::string GetMessage() const;
};

class UserResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  uint64_t GetID() const;

  std::string GetName() const;
};

class UsersResult : public APIResult {
 public:
  class User {
   public:
    explicit User(const rapidjson::Value& data) : data_(data) {}

    uint64_t GetID() const;

   private:
    const rapidjson::Value& data_;
  };

  rapidjson::Document GetSchema() override final;

  bool IsEmpty() const;

  size_t GetSize() const;

  const UsersResult::User GetUser(const size_t index) const;
};

class AuthTokenResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  std::string GetAccessToken() const;
};

class StreamsResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  bool IsEmpty() const;

  uint64_t GetViewers() const;

  std::string GetLargePreview() const;
};

class ChannelsResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  std::string GetVideoBanner() const;
};

class VideosResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  std::string GetLargePreview() const;

  uint64_t GetViews() const;
};

struct ClientConfig {
  const std::string client_id;
  const std::string client_secret;
  const std::string redirect_uri;
};

class Client {
 public:
  explicit Client(ClientConfig config);

  Status GetOAuthToken(const std::string& code, AuthTokenResult* result);

  Status GetUserByOAuthToken(const std::string& token, UserResult* result);

  Status GetUsersByName(const std::string& name, UsersResult* result);

  Status GetStreamByID(const uint64_t channel_id, StreamsResult* result);

  Status GetChannelByID(const uint64_t channel_id, ChannelsResult* result);

  Status GetVideosByID(const std::string& video_id, VideosResult* result);

 private:
  inline std::string GetKrakenURL(const std::string& path) {
    return "https://api.twitch.tv/kraken/" + path;
  }

  template <typename T>
  Status LoadResultFromURL(const std::string& url, T* result) {
    CurlRequest req(url);
    req.AddHeader("Accept: application/vnd.twitchtv.v5+json");
    req.AddHeader("Client-ID: " + config_.client_id);
    req.Submit();

    return LoadResultFromCurlRequest(req, result);
  }

  template <typename T>
  Status LoadResultFromCurlRequest(const CurlRequest& req, T* result) {
    if (!req.Ok()) {
      return Status(StatusCode::HTTP_ERROR, req.GetErrorMessage());
    }

    const auto& response = req.GetResponse();

    if (req.GetResponseCode() != 200) {
      ErrorResult error;
      if (error.SetData(response.c_str(), response.size()).Ok()) {
        return Status(StatusCode::API_ERROR, error.GetError(),
                      error.GetMessage());
      }
      return Status::ERROR;
    }

    return result->SetData(response.c_str(), response.size());
  }

  ClientConfig config_;
};

}  // namespace twitch
}  // namespace rustla2
