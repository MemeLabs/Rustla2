#pragma once

#include <string>

#include "APIClient.h"

namespace rustla2 {
namespace okru {

class ChannelResult : public APIResult {
 public:
  rapidjson::Document GetSchema() final;

  std::string GetTitle() const;

  bool GetLive() const;

  std::string GetThumbnail() const;
};

class Client {
 public:
  Status GetChannelByID(const std::string& id, ChannelResult* result);
};

}  // namespace okru
}  // namespace rustla2
