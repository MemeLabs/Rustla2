#pragma once

#include <folly/Format.h>
#include <folly/String.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <functional>
#include <memory>
#include <string>

#include "Status.h"

namespace rustla2 {

constexpr folly::StringPiece kAdvancedService{"advanced"};
constexpr folly::StringPiece kAfreecaService{"afreeca"};
constexpr folly::StringPiece kAngelThumpService{"angelthump"};
constexpr folly::StringPiece kAzubuService{"azubu"};
constexpr folly::StringPiece kDailyMotionService{"dailymotion"};
constexpr folly::StringPiece kFacebookService{"facebook"};
constexpr folly::StringPiece kHitboxService{"hitbox"};
constexpr folly::StringPiece kHitboxVODService{"hitbox-vod"};
constexpr folly::StringPiece kMLGService{"mlg"};
constexpr folly::StringPiece kNSFWChaturbateService{"nsfw-chaturbate"};
constexpr folly::StringPiece kStreamupService{"streamup"};
constexpr folly::StringPiece kTwitchService{"twitch"};
constexpr folly::StringPiece kTwitchVODService{"twitch-vod"};
constexpr folly::StringPiece kUstreamService{"ustream"};
constexpr folly::StringPiece kVaughnService{"vaughn"};
constexpr folly::StringPiece kYouTubePlaylistService{"youtube-playlist"};
constexpr folly::StringPiece kYouTubeService{"youtube"};

constexpr std::array<folly::StringPiece, 17> kServices{
    kAdvancedService,        kAfreecaService,     kAngelThumpService,
    kAzubuService,           kDailyMotionService, kFacebookService,
    kHitboxService,          kHitboxVODService,   kMLGService,
    kNSFWChaturbateService,  kStreamupService,    kTwitchService,
    kTwitchVODService,       kUstreamService,     kVaughnService,
    kYouTubePlaylistService, kYouTubeService};

constexpr std::array<folly::StringPiece, 4> kCaseInsensitiveServices{
    kAngelThumpService, kHitboxService, kTwitchService, kUstreamService};

class Channel {
 public:
  static Channel Create(const std::string &channel, const std::string &service,
                        const std::string &stream_path, Status *status);

  static Channel Create(const std::string &channel, const std::string &service,
                        const std::string &stream_path = "") {
    Status status;
    return Create(channel, service, stream_path, &status);
  }

  static Channel Create(const std::string &channel, const std::string &service,
                        Status *status) {
    return Create(channel, service, "", status);
  }

  inline operator std::shared_ptr<Channel>() const {
    return std::shared_ptr<Channel>(
        new Channel(channel_, service_, stream_path_));
  }

  inline const std::string &GetChannel() const { return channel_; }

  inline const std::string &GetService() const { return service_; }

  inline const std::string GetPath() const {
    return !stream_path_.empty() ? stream_path_
                                 : folly::sformat("/{}/{}", service_, channel_);
  }

  inline const std::string &GetStreamPath() const { return stream_path_; }

  inline bool HasStreamPath() const { return !stream_path_.empty(); }

  inline bool IsEmpty() { return channel_.empty() || service_.empty(); }

  void WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

 private:
  Channel() {}

  Channel(const std::string &channel, const std::string &service,
          const std::string &stream_path)
      : channel_(channel), service_(service), stream_path_(stream_path) {}

  Status Init(const std::string &channel, const std::string &service,
              const std::string &stream_path);

  Status ValidateService(const std::string &service);

  Status ValidlatePath(const std::string &stream_path);

  Status NormalizeChannel(const std::string &service,
                          std::string *clean_channel);

  Status NormalizeAdvancedChannel(std::string *clean_channel);

  Status NormalizeBasicChannel(const std::string &service,
                               std::string *clean_channel);

  std::string channel_;
  std::string service_;
  std::string stream_path_;

  friend std::ostream &operator<<(std::ostream &os, const Channel &channel);
  friend class ChannelHash;
  friend class ChannelEqual;
  friend class ChannelSourceHash;
  friend class ChannelSourceEqual;
};

struct ChannelHash : public std::unary_function<Channel, std::size_t> {
  std::size_t operator()(const Channel &k) const {
    return std::hash<std::string>{}(k.channel_) ^
           std::hash<std::string>{}(k.service_) ^
           std::hash<std::string>{}(k.stream_path_);
  }
};

struct ChannelEqual : public std::binary_function<Channel, Channel, bool> {
  bool operator()(const Channel &a, const Channel &b) const {
    return a.channel_ == b.channel_ && a.service_ == b.service_ &&
           a.stream_path_ == b.stream_path_;
  }
};

struct ChannelSourceHash : public std::unary_function<Channel, std::size_t> {
  std::size_t operator()(const Channel &k) const {
    return std::hash<std::string>{}(k.channel_) ^
           std::hash<std::string>{}(k.service_);
  }
};

struct ChannelSourceEqual
    : public std::binary_function<Channel, Channel, bool> {
  bool operator()(const Channel &a, const Channel &b) const {
    return a.channel_ == b.channel_ && a.service_ == b.service_;
  }
};

}  // namespace rustla2
