#pragma once

#include <rapidjson/document.h>
#include <string>

#include "APIClient.h"
#include "Curl.h"
#include "Status.h"

namespace rustla2 {
namespace smashcast {

class ChannelResult : public APIResult {
  public:
    rapidjson::Document GetSchema() final;

    std::string GetTitle() const;

    bool GetLive() const;

    std::string GetThumbnail() const;

    uint64_t GetViewers() const;
};

class Client {
  public:
    Status GetChannelByName(const std::string& name, ChannelResult* result);
};

} // namespace smashcast
} // namespace rustla2
