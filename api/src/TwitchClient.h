#pragma once

#include <rapidjson/document.h>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <chrono>
#include <string>

#include "APIClient.h"
#include "Curl.h"
#include "Status.h"

namespace rustla2 {
namespace twitch {

class ErrorResult : public APIResult {
 public:
  rapidjson::Document GetSchema() final;

  std::string GetError() const;

  std::string GetMessage() const;
};

class UsersResult : public APIResult {
 public:
  class User {
   public:
    explicit User(const rapidjson::Value& data) : data_(data) {}

    uint64_t GetID() const;

    std::string GetName() const;

    std::string GetOfflineImageURL() const;

   private:
    const rapidjson::Value& data_;
  };

  rapidjson::Document GetSchema() final;

  bool IsEmpty() const;

  size_t GetSize() const;

  const UsersResult::User GetUser(const size_t index) const;
};

class AuthTokenResult : public APIResult {
 public:
  rapidjson::Document GetSchema() final;

  std::string GetAccessToken() const;

  uint64_t GetExpiresIn() const;
};

class StreamsResult : public APIResult {
 public:
  rapidjson::Document GetSchema() final;

  const rapidjson::Value& GetStreamData() const;

  bool IsEmpty() const;

  std::string GetGame() const;

  uint64_t GetViewers() const;

  std::string GetThumbnailURL() const;
};

class VideosResult : public APIResult {
 public:
  rapidjson::Document GetSchema() final;

  const rapidjson::Value& GetVideoData() const;

  std::string GetTitle() const;

  std::string GetThumbnailURL() const;

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

  Status GetUserByOAuthToken(const std::string& token, UsersResult* result);

  Status GetUsersByName(const std::string& name, UsersResult* result);

  Status GetStreamByID(const uint64_t channel_id, StreamsResult* result);

  Status GetVideosByID(const std::string& video_id, VideosResult* result);

 private:
  inline std::string GetHelixURL(const std::string& path) {
    return "https://api.twitch.tv/helix/" + path;
  }

  template <typename T>
  Status LoadResultFromURL(const std::string& url, T* result) {
    CurlRequest req(url);
    req.AddHeader("Client-Id: " + config_.client_id);
    req.AddHeader("Authorization: Bearer " + GetAccessToken());
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

  Status GetServerOAuthToken(AuthTokenResult* result);

  std::string GetAccessToken();

  ClientConfig config_;
  boost::mutex access_token_lock_;
  std::chrono::time_point<std::chrono::steady_clock> access_token_eol_{};
  std::string access_token_ = "";
};

}  // namespace twitch
}  // namespace rustla2
