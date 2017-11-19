#pragma once

#include <rapidjson/document.h>
#include <string>

#include "APIClient.h"
#include "Config.h"
#include "Status.h"

namespace rustla2 {
namespace youtube {

class VideosResult : public APIResult {
 public:
  class Video {
   public:
    explicit Video(const rapidjson::Value& data) : data_(data) {}

    uint64_t GetViewers() const;

    std::string GetMediumThumbnail() const;

   private:
    const rapidjson::Value& data_;
  };

  rapidjson::Document GetSchema() override final;

  bool IsEmpty() const;

  uint64_t GetTotalResults() const;

  const VideosResult::Video GetVideo(const size_t index) const;
};

class ErrorResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  uint64_t GetErrorCode() const;

  std::string GetMessage() const;
};

struct ClientConfig {
  const std::string public_api_key;
};

class Client {
 public:
  explicit Client(ClientConfig config) : config_(config) {}

  Status GetVideosByID(const std::string& id, VideosResult* result);

 private:
  ClientConfig config_;
};

}  // namespace youtube
}  // namespace rustla2
