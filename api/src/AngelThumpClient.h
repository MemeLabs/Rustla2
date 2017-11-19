#pragma once

#include <rapidjson/document.h>
#include <string>

#include "APIClient.h"

namespace rustla2 {
namespace angelthump {

class ChannelResult : public APIResult {
 public:
  rapidjson::Document GetSchema() override final;

  bool GetLive() const;

  std::string GetThumbnail() const;

  uint64_t GetViewers() const;
};

class Client {
 public:
  Status GetChannelByName(const std::string& name, ChannelResult* result);
};

}  // namespace angelthump
}  // namespace rustla2
