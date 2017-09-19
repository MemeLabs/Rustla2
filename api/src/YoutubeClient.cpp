#include "YoutubeClient.h"

#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <sstream>

#include "Curl.h"

namespace rustla2 {
namespace youtube {

uint64_t VideosResult::Video::GetViewers() const {
  const auto& viewers = data_["liveStreamingDetails"]["concurrentViewers"];
  return std::stoull(
      std::string(viewers.GetString(), viewers.GetStringLength()));
}

std::string VideosResult::Video::GetMediumThumbnail() const {
  const auto& preview = data_["snippet"]["thumbnails"]["medium"]["url"];
  return std::string(preview.GetString(), preview.GetStringLength());
}

rapidjson::Document VideosResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "pageInfo": {
            "type": "object",
            "properties": {
              "totalResults": {"type": "integer"}
            },
            "required": ["totalResults"]
          },
          "items": {
            "type": "array",
            "items": {
              "type": "object",
              "properties": {
                "snippet": {
                  "type": "object",
                  "properties": {
                    "thumbnails": {
                      "type": "object",
                      "properties": {
                        "medium": {
                          "type": "object",
                          "properties": {
                            "url": {
                              "type": "string",
                              "format": "uri"
                            }
                          },
                          "required": ["url"]
                        }
                      },
                      "required": ["medium"]
                    }
                  },
                  "required": ["thumbnails"]
                },
                "liveStreamingDetails": {
                  "type": "object",
                  "properties": {
                    "concurrentViewers": {
                      "type": "string",
                      "pattern": "^[0-9]+$"
                    }
                  },
                  "required": ["concurrentViewers"]
                }
              },
              "required": ["snippet", "liveStreamingDetails"]
            }
          }
        }
      }
    )json");
  return schema;
}

bool VideosResult::IsEmpty() const { return GetTotalResults() == 0; }

uint64_t VideosResult::GetTotalResults() const {
  return GetData()["pageInfo"]["totalResults"].GetUint64();
}

const VideosResult::Video VideosResult::GetVideo(const size_t index) const {
  const auto& items = GetData()["items"].GetArray();
  return Video(items[index]);
}

rapidjson::Document ErrorResult::GetSchema() {
  rapidjson::Document schema;
  schema.Parse(R"json(
      {
        "type": "object",
        "properties": {
          "error": {
            "type": "object",
            "properties": {
              "code": {"type": "integer"},
              "message": {"type": "string"}
            },
            "required": ["code", "message"]
          }
        },
        "required": ["error"]
      }
    )json");
  return schema;
}

uint64_t ErrorResult::GetErrorCode() const {
  return GetData()["error"]["code"].GetUint64();
}

std::string ErrorResult::GetMessage() const {
  const auto& message = GetData()["error"]["message"];
  return std::string(message.GetString(), message.GetStringLength());
}

APIStatus Client::GetVideosByID(const std::string& id, VideosResult* result) {
  std::stringstream url;
  url << "https://www.googleapis.com/youtube/v3/videos"
      << "?key=" << config_.public_api_key
      << "&part=liveStreamingDetails,snippet"
      << "&id=" << id;

  CurlRequest req(url.str());
  req.Submit();

  if (!req.Ok()) {
    return APIStatus(StatusCode::HTTP_ERROR, req.GetErrorMessage());
  }

  const auto& response = req.GetResponse();

  if (req.GetResponseCode() != 200) {
    ErrorResult error;
    if (error.SetData(response.c_str(), response.size()).Ok()) {
      return APIStatus(
          StatusCode::API_ERROR,
          "received error code " + std::to_string(error.GetErrorCode()),
          error.GetMessage());
    }
    return APIStatus::ERROR;
  }

  return result->SetData(response.c_str(), response.size());
}

}  // namespace youtube
}  // namespace rustla2
