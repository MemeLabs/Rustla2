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
          "name": {"type": "string"},
          "audience": {"type": "string"},
          "online": {"type": "boolean"},
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
        "required": ["name", "online", "thumbnail", "viewersCurrent", "audience"]
      }
    )json");
  return schema;
}

std::string ChannelResult::GetName() const {
  return json::StringRef(GetData()["name"]);
}

bool ChannelResult::GetLive() const { return GetData()["online"].GetBool(); }

bool ChannelResult::IsNSFW() const {
  return json::StringRef(GetData()["audience"]) == "18+";
}

std::string ChannelResult::GetThumbnail() const {
  return json::StringRef(GetData()["thumbnail"]["url"]);
}

uint64_t ChannelResult::GetViewers() const {
  return GetData()["viewersCurrent"].GetUint64();
}

Status Client::GetChannelByName(const std::string &name,
                                ChannelResult *result) {
  CurlRequest req("https://mixer.com/api/v1/channels/" + name);
  req.Submit();

  if (!req.Ok()) {
    return Status(StatusCode::HTTP_ERROR, req.GetErrorMessage());
  }
  if (req.GetResponseCode() != 200) {
    return Status(StatusCode::API_ERROR, "recieve non 200 response",
                  "api returned status code " +
                      std::to_string(req.GetResponseCode()));
  }

  const auto &response = req.GetResponse();
  return result->SetData(response.c_str(), response.size());
}

} // namespace mixer
} // namespace rustla2
