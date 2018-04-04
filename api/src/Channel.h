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
                        Status *status);

  static Channel Create(const std::string &channel,
                        const std::string &service) {
    Status status;
    return Create(channel, service, &status);
  }

  inline operator std::shared_ptr<Channel>() const {
    return std::shared_ptr<Channel>(new Channel(channel_, service_));
  }

  inline const std::string &GetChannel() const { return channel_; }

  inline const std::string &GetService() const { return service_; }

  inline const std::string GetPath() const {
    return folly::sformat("/{}/{}", service_, channel_);
  }

  inline bool IsEmpty() { return channel_.empty() || service_.empty(); }

  void WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

 private:
  Channel() {}

  Channel(const std::string &channel, const std::string &service)
      : channel_(channel), service_(service) {}

  Status Init(const std::string &channel, const std::string &service);

  bool IsValidService(const std::string &service);

  Status NormalizeChannel(const std::string &service,
                          std::string *clean_channel);

  Status NormalizeAdvancedChannel(std::string *clean_channel);

  Status NormalizeBasicChannel(const std::string &service,
                               std::string *clean_channel);

  std::string channel_;
  std::string service_;

  friend std::ostream &operator<<(std::ostream &os, const Channel &channel);
};

struct ChannelHash : public std::unary_function<Channel, std::size_t> {
  std::size_t operator()(const Channel &k) const {
    return std::hash<std::string>{}(k.GetChannel()) ^
           std::hash<std::string>{}(k.GetService());
  }
};

struct ChannelEqual : public std::binary_function<Channel, Channel, bool> {
  bool operator()(const Channel &a, const Channel &b) const {
    return a.GetChannel() == b.GetChannel() && a.GetService() == b.GetService();
  }
};

}  // namespace rustla2
