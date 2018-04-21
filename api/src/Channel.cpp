#include "Channel.h"

#include <algorithm>
#include <iostream>

#include <folly/Uri.h>
#include <boost/regex.hpp>

namespace rustla2 {

Channel Channel::Create(const std::string &channel, const std::string &service,
                        const std::string &stream_path, Status *status) {
  Channel instance;
  *status = instance.Init(channel, service, stream_path);
  return instance;
}

Status Channel::Init(const std::string &channel, const std::string &service,
                     const std::string &stream_path) {
  auto status = ValidateService(service);

  std::string normalized_channel(channel);
  if (status.Ok()) {
    status = NormalizeChannel(service, &normalized_channel);
  }

  if (status.Ok()) {
    status = ValidlatePath(stream_path);
  }

  if (status.Ok()) {
    service_ = service;
    channel_ = normalized_channel;
    stream_path_ = stream_path;
  }

  return status;
}

Status Channel::ValidateService(const std::string &service) {
  auto it = std::find(kServices.begin(), kServices.end(), service);
  if (it == kServices.end()) {
    return Status(StatusCode::VALIDATION_ERROR, "invalid service");
  }

  return Status::OK;
}

Status Channel::ValidlatePath(const std::string &stream_path) {
  if (stream_path.empty()) {
    return Status::OK;
  }

  const boost::regex valid_stream_path_regex("^[a-z0-9_]{3,32}$");
  if (!boost::regex_match(stream_path, valid_stream_path_regex)) {
    return Status(
        StatusCode::VALIDATION_ERROR,
        "Stream path may only contain a-z 0-9 or underscores and must "
        "be betwee 3 and 32 characters in length.");
  }

  return Status::OK;
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
  writer->Key("path");
  writer->String(stream_path_);
  writer->EndObject();
}

std::ostream &operator<<(std::ostream &os, const Channel &channel) {
  os << "service: " << channel.service_ << ", "
     << "channel: " << channel.channel_ << ", "
     << "stream_path: " << channel.stream_path_;
  return os;
}

}  // namespace rustla2
