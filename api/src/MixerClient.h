#pragma once

#include <rapidjson/document.h>
#include <string>

#include "APIClient.h"
#include "Curl.h"
#include "Status.h"

namespace rustla2 {
namespace mixer {

class ChannelResult : public APIResult {
 public:
  rapidjson::Document GetSchema() final;

  std::string GetName() const;

  bool GetLive() const;

  bool IsNSFW() const;

  std::string GetThumbnail() const;

  uint64_t GetViewers() const;
};

class Client {
 public:
  Status GetChannelByName(const std::string& name, ChannelResult* result);
};

} // namespace mixer
} // namespace rustla2
