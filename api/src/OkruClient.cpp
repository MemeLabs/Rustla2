#include "OkruClient.h"

#include "Curl.h"
#include "JSON.h"

namespace rustla2 {
namespace okru {

rapidjson::Document ChannelResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "movie": {
            "type": "object",
            "properties": {
              "status": {"type": "string"},
              "isLive": {"type": "boolean"},
              "title": {"type": ["string", "null"]},
              "poster": {"type": ["string", "null"]}
            },
            "required": ["status", "isLive"]
          }
        },
        "required": ["movie"]
      }
    )json");
  return schema;
}

std::string ChannelResult::GetTitle() const {
  const auto& movie = GetData()["movie"];
  if (movie.HasMember("title") && movie["title"].IsString()) {
    return json::StringRef(movie["title"]);
  }
  return "";
}

bool ChannelResult::GetLive() const {
  const auto& movie = GetData()["movie"];
  // Paused broadcasts and recordings can also have isLive set to true.
  return movie["isLive"].GetBool() &&
         json::StringRef(movie["status"]) == "ONLINE";
}

std::string ChannelResult::GetThumbnail() const {
  const auto& movie = GetData()["movie"];
  if (movie.HasMember("poster") && movie["poster"].IsString()) {
    return json::StringRef(movie["poster"]);
  }
  return "";
}

Status Client::GetChannelByID(const std::string& id, ChannelResult* result) {
  if (id.empty() || id.find_first_not_of("0123456789") != std::string::npos) {
    return Status(StatusCode::VALIDATION_ERROR, "invalid OK.ru broadcast ID");
  }

  CurlRequest req("https://ok.ru/dk?cmd=videoPlayerMetadata&mid=" + id);
  // The player metadata endpoint requires POST, even with an empty body.
  req.SetPostData("", 0);
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

}  // namespace okru
}  // namespace rustla2
