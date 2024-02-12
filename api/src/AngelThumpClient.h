#pragma once

#include <rapidjson/document.h>

#include <string>

#include "APIClient.h"

namespace rustla2 {
namespace angelthump {

class ChannelResult : public APIResult {
 public:
  rapidjson::Document GetSchema() final;

  const rapidjson::Value& GetVideoData() const;

  std::string GetTitle() const;

  bool GetLive() const;

  std::string GetThumbnail() const;

  uint64_t GetViewers() const;

  bool IsNSFW() const;
};

class Client {
 public:
  Status GetChannelByName(const std::string& name, ChannelResult* result);
};

}  // namespace angelthump
}  // namespace rustla2
