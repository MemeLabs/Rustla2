#include "SmashcastClient.h"

#include "Curl.h"
#include "JSON.h"

namespace rustla2 {
namespace smashcast {
rapidjson::Document ChannelResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "media_title": {"type": "string"},
          "media_is_live": {"type": "integer"},
          "media_thumbnail": {"type": "string"},
          "media_views": {"type": "integer"}
        },
        "required": ["media_title", "media_is_live", "media_thumbnail", "media_views"]
      }
    )json");
  return schema;
}

std::string ChannelResult::GetTitle() const {
  return json::StringRef(GetData()["media_title"]);
}

bool ChannelResult::GetLive() const {
  if(GetData().HasMember("media_is_live") && GetData()["media_is_live"].GetUint64() == 1) {
    return true;
  }
  return false;
}

std::string ChannelResult::GetThumbnail() const {
  return json::StringRef(GetData()["media_thumbnail"]);
}

uint64_t ChannelResult::GetViewers() const {
  return GetData().HasMember("media_views") ? GetData()["media_views"].GetUint64() : 0;
}

Status Client::GetChannelByName(const std::string& name, ChannelResult* result) {
  CurlRequest req("https://api.smashcast.tv/media/live/" + name);
  req.Submit();

  if(!req.Ok()) {
    return Status(StatusCode::HTTP_ERROR, req.GetErrorMessage());
  }

  if(req.GetResponseCode() != 200) {
    return Status(
        StatusCode::API_ERROR, "received non 200 response",
        "api returned status code " + std::to_string(req.GetResponseCode()));
  }

  const auto& response = req.GetResponse();
  return result->SetData(response.c_str(), response.size());
}

} // namespace smashcast
} // namespace rustla2
