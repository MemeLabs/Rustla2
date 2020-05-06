#include "AngelThumpClient.h"

#include "Curl.h"
#include "JSON.h"

namespace rustla2 {
namespace angelthump {

rapidjson::Document ChannelResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
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
    )json");
  return schema;
}

std::string ChannelResult::GetTitle() const {
  if (GetData()["user"].HasMember("title")) {
    return json::StringRef(GetData()["user"]["title"]);
  }
  return "";
}

bool ChannelResult::GetLive() const {
  return GetData().HasMember("type") &&
         json::StringRef(GetData()["type"]) == "live";
}

std::string ChannelResult::GetThumbnail() const {
  return GetData().HasMember("thumbnail_url")
             ? json::StringRef(GetData()["thumbnail_url"])
             : json::StringRef(GetData()["user"]["offline_banner_url"]);
}

bool ChannelResult::IsNSFW() const {
  return GetData()["user"]["nsfw"].GetBool();
}

uint64_t ChannelResult::GetViewers() const {
  return GetData().HasMember("viewer_count")
             ? GetData()["viewer_count"].GetUint64()
             : 0;
}

Status Client::GetChannelByName(const std::string& name,
                                ChannelResult* result) {
  CurlRequest req("https://api.angelthump.com/v2/streams/" + name);
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
