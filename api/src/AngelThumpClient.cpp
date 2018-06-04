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
          "title": {"type": "string"},
          "live": {"type": "boolean"},
          "poster": {
            "type": "string",
            "format": "uri"
          },
          "thumbnail": {
            "type": "string",
            "format": "uri"
          },
          "viewers": {"type": "integer"}
        },
        "required": ["poster"]
      }
    )json");
  return schema;
}

std::string ChannelResult::GetTitle() const {
  if (GetData().HasMember("title")) {
    return json::StringRef(GetData()["title"]);
  }
  return "";
}

bool ChannelResult::GetLive() const {
  return GetData().HasMember("live") && GetData()["live"].GetBool();
}

std::string ChannelResult::GetThumbnail() const {
  return GetData().HasMember("thumbnail")
             ? json::StringRef(GetData()["thumbnail"])
             : json::StringRef(GetData()["poster"]);
}

uint64_t ChannelResult::GetViewers() const {
  return GetData().HasMember("viewers") ? GetData()["viewers"].GetUint64() : 0;
}

Status Client::GetChannelByName(const std::string& name,
                                ChannelResult* result) {
  CurlRequest req("https://angelthump.com/api/" + name);
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
