#include "Channel.h"

#include <folly/Uri.h>
#include <algorithm>
#include <boost/regex.hpp>

namespace rustla2 {

Channel Channel::Create(const std::string &channel, const std::string &service,
                        Status *status) {
  Channel instance;
  *status = instance.Init(channel, service);
  return instance;
}

Status Channel::Init(const std::string &channel, const std::string &service) {
  if (!IsValidService(service)) {
    return Status(StatusCode::VALIDATION_ERROR, "invalid service");
  }

  std::string normalized_channel(channel);
  auto status = NormalizeChannel(service, &normalized_channel);

  if (status.Ok()) {
    service_ = service;
    channel_ = normalized_channel;
  }

  return status;
}

bool Channel::IsValidService(const std::string &service) {
  auto it = std::find(kServices.begin(), kServices.end(), service);
  return it != kServices.end();
}

Status Channel::NormalizeChannel(const std::string &service,
                                 std::string *channel) {
  return service == kAdvancedService ? NormalizeAdvancedChannel(channel)
                                     : NormalizeBasicChannel(service, channel);
}

Status Channel::NormalizeAdvancedChannel(std::string *channel) {
  try {
    auto channel_uri = folly::Uri(*channel);

    if (channel_uri.scheme() != "http" && channel_uri.scheme() != "https") {
      return Status(StatusCode::VALIDATION_ERROR,
                    "invalid advanced url schema. must be http or https");
    }

    *channel = channel_uri.str();
  } catch (const std::invalid_argument &e) {
    return Status(StatusCode::VALIDATION_ERROR, "invalid advanced url");
  }

  return Status::OK;
}

Status Channel::NormalizeBasicChannel(const std::string &service,
                                      std::string *channel) {
  const boost::regex valid_channel_regex("^[a-zA-Z0-9\\-_]{1,64}$");
  if (!boost::regex_match(*channel, valid_channel_regex)) {
    return Status(StatusCode::VALIDATION_ERROR, "invalid channel");
  }

  auto it = std::find(kCaseInsensitiveServices.begin(),
                      kCaseInsensitiveServices.end(), service);
  if (it != kCaseInsensitiveServices.end()) {
    folly::toLowerAscii(*channel);
  }

  return Status::OK;
}

void Channel::WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  writer->StartObject();
  writer->Key("channel");
  writer->String(channel_);
  writer->Key("service");
  writer->String(service_);
  writer->EndObject();
}

}  // namespace rustla2