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
          "livestream": {
            "type": "array",
            "items": {
              "type": "object",
              "properties": {
                "media_title": {"type": "string"},
                "media_is_live": {"type": "string"},
                "media_thumbnail": {"type": "string"},
                "media_views": {
                  "type": "string",
                  "pattern": "^[0-9]+$"
                }
              },
              "required": [
                "media_title",
                "media_is_live",
                "media_thumbnail",
                "media_views"
              ]
            },
            "minItems": 1
          }
        },
        "required": ["livestream"]
      }
    )json");
  return schema;
}

std::string ChannelResult::GetTitle() const {
  return json::StringRef(GetLivestream()["media_title"]);
}

bool ChannelResult::GetLive() const {
  return json::StringRef(GetLivestream()["media_is_live"]) == "1";
}

std::string ChannelResult::GetThumbnail() const {
  return "https://edge.sf.hitbox.tv" +
         std::string(json::StringRef(GetLivestream()["media_thumbnail"]));
}

uint64_t ChannelResult::GetViewers() const {
  return std::stoull(
      std::string(json::StringRef(GetLivestream()["media_views"])));
}

const rapidjson::Value& ChannelResult::GetLivestream() const {
  return GetData()["livestream"][0];
}

Status Client::GetChannelByName(const std::string& name,
                                ChannelResult* result) {
  CurlRequest req("https://api.smashcast.tv/media/live/" + name);
  req.Submit();

  if (!req.Ok()) {
    return Status(StatusCode::HTTP_ERROR, req.GetErrorMessage());
  }

  if (req.GetResponseCode() != 200) {
    return Status(
        StatusCode::API_ERROR, "received non 200 response",
        "api returned status code " + std::to_string(req.GetResponseCode()));
  }

  const auto& response = req.GetResponse();
  return result->SetData(response.c_str(), response.size());
}

}  // namespace smashcast
}  // namespace rustla2
