#include "AngelThumpClient.h"

#include "Curl.h"
#include "JSON.h"

namespace rustla2 {
namespace angelthump {

rapidjson::Document ChannelResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "array",
        "minItems": 1,
        "items": {
          "type": "object",
          "properties": {
            "type": {"type": "string"},
            "thumbnail_url": {
              "type": "string",
              "format": "uri"
            },
            "viewer_count": {"type": "integer"},
            "user": {
              "type": "object",
              "properties": {
                "title": {"type": "string"},
                "offline_banner_url": {
                  "type": "string",
                  "format": "uri"
                },
                "nsfw": {"type": "boolean"}
              },
              "required": ["offline_banner_url", "nsfw"]
            }
          },
          "required": ["user"]
        }
      }
    )json");
  return schema;
}

const rapidjson::Value& ChannelResult::GetVideoData() const {
  return GetData().GetArray()[0];
}

std::string ChannelResult::GetTitle() const {
  if (GetVideoData()["user"].HasMember("title")) {
    return json::StringRef(GetVideoData()["user"]["title"]);
  }
  return "";
}

bool ChannelResult::GetLive() const {
  return GetVideoData().HasMember("type") &&
         json::StringRef(GetVideoData()["type"]) == "live";
}

std::string ChannelResult::GetThumbnail() const {
  return GetVideoData().HasMember("thumbnail_url")
             ? json::StringRef(GetVideoData()["thumbnail_url"])
             : json::StringRef(GetVideoData()["user"]["offline_banner_url"]);
}

bool ChannelResult::IsNSFW() const {
  return GetVideoData()["user"]["nsfw"].GetBool();
}

uint64_t ChannelResult::GetViewers() const {
  return GetVideoData().HasMember("viewer_count")
             ? GetVideoData()["viewer_count"].GetUint64()
             : 0;
}

Status Client::GetChannelByName(const std::string& name,
                                ChannelResult* result) {
  CurlRequest req("https://api.angelthump.com/v3/streams?username=" + name);
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

}  // namespace angelthump
}  // namespace rustla2
