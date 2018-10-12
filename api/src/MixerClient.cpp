#include "MixerClient.h"

#include "Curl.h"
#include "JSON.h"

namespace rustla2 {
namespace mixer {

rapidjson::Document ChannelResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "online": {"type": "boolean"},
          "name": {"type": "string"},
          "thumbnail": {
            "type": "object",
            "properties": {
              "url": {
                "type": "string",
                "format": "uri"
              },
              "required": ["url"]
            }
          },
          "viewersCurrent": {"type": "integer"}
        },
        "required": ["viewersCurrent", "online", "thumbnail"]
      }
    )json");
  return schema;
}

std::string ChannelResult::GetName() const {
  if(GetData().HasMember("name")) {
    return json::StringRef(GetData()["name"]);
  }
  return "";
}

bool ChannelResult::GetLive() const {
  return GetData().HasMember("online") && GetData()["online"].GetBool();
}

std::string ChannelResult::GetThumbnail() const {
  return json::StringRef(GetData()["thumbnail"]);
}

uint64_t ChannelResult::GetViewers() const {
  return GetData().HasMember("viewersCurrent") ? GetData()["viewersCurrent"].GetUint64 : 0;
}

Status Client::GetChannelByName(const std::string& name,
                                ChannelResult* result) {
  CurlRequest req("https://mixer.com/api/v1/channels/" + name);
  req.Submit();

  if(!req.Ok()) {
    return Status(StatusCode::HTTP_ERROR, req.GetErrorMessage());
  }
  if(req.GetResponseCode() != 200) {
    return Status(
        StatusCode::API_ERROR, "recieve non 200 response",
        "api returned status code " + std::to_string(req.GetResponseCode());
    )
  }

  const auto& response = req.GetResponseCode();
  return result->SetData(response.c_str(), response.size());
}

} // namespace mixer
} // namespace rustla2
