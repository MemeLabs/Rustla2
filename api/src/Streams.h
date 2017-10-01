#pragma once

#include <folly/String.h>
#include <glog/logging.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sqlite_modern_cpp.h>
#include <boost/thread/shared_mutex.hpp>
#include <chrono>
#include <memory>
#include <unordered_map>

namespace rustla2 {

enum UpdateResponse { PARAM_ERROR, DB_ERROR, SUCCESS };

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

// Cap stream IDs to 48 bits for JS
const uint64_t kMaxStreamID = 0xFFFFFFFFFFF;

using Channel = std::tuple<std::string, std::string>;

struct ChannelHash : public std::unary_function<Channel, std::size_t> {
  std::size_t operator()(const Channel &k) const {
    return std::hash<std::string>{}(std::get<0>(k)) ^
           std::hash<std::string>{}(std::get<1>(k));
  }
};

struct ChannelEqual : public std::binary_function<Channel, Channel, bool> {
  bool operator()(const Channel &a, const Channel &b) const {
    return std::get<0>(a) == std::get<0>(b) && std::get<1>(a) == std::get<1>(b);
  }
};

class Stream {
 public:
  Stream(sqlite::database db, const uint64_t id, const std::string &channel,
         const std::string &service, const std::string &overrustle_id,
         const std::string &thumbnail = "", const bool live = false,
         const uint64_t viewer_count = 0)
      : db_(db),
        id_(id),
        overrustle_id_(overrustle_id),
        thumbnail_(thumbnail),
        live_(live),
        nsfw_(false),
        path_("/" + service + "/" + channel),
        viewer_count_(viewer_count) {
    SetChannelAndService(channel, service);
  }

  Stream(sqlite::database db, const std::string &channel,
         const std::string &service, const std::string &overrustle_id)
      : Stream(db,
               ChannelHash{}(std::make_tuple(channel, service)) & kMaxStreamID,
               channel, service, overrustle_id) {}

  inline uint64_t GetID() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return id_;
  }

  inline std::string GetChannel() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return channel_;
  }

  inline std::string GetService() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return service_;
  }

  inline std::string GetOverrustleID() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return overrustle_id_;
  }

  inline std::string GetThumbnail() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return thumbnail_;
  }

  inline bool GetLive() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return live_;
  }

  inline bool GetNSFW() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return nsfw_;
  }

  inline std::string GetPath() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return path_;
  }

  inline uint64_t GetViewerCount() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return viewer_count_;
  }

  inline uint64_t GetRustlerCount() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return rustler_count_;
  }

  inline uint64_t GetUpdateTime() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return update_time_;
  }

  inline uint64_t GetResetTime() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return reset_time_;
  }

  void WriteAPIJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

  void WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

  inline uint64_t IncrRustlerCount() {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    update_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count();

    if (rustler_count_ == 0) {
      reset_time_ = update_time_;
    }

    return ++rustler_count_;
  }

  inline uint64_t DecrRustlerCount() {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);

    if (rustler_count_ == 0) {
      LOG(WARNING) << "DecrRustlerCount called on stream with 0 rustlers";
      return 0;
    }

    update_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
                       std::chrono::steady_clock::now().time_since_epoch())
                       .count();

    return --rustler_count_;
  }

  bool GetIsValid() { return !channel_.empty() && !service_.empty(); }

  bool SetChannelAndService(const std::string &channel,
                            const std::string &service);

  inline bool SetLive(const bool live) {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    live_ = live;
    return true;
  }

  inline bool SetThumbnail(const std::string thumbnail) {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    thumbnail_ = thumbnail;
    return true;
  }

  inline bool SetViewerCount(const uint64_t viewer_count) {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    viewer_count_ = viewer_count;
    return true;
  }

  inline bool SetNSFW(const bool nsfw) {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    nsfw_ = nsfw;
    return true;
  }

  static bool SanitizeChannel(const std::string &channel,
                              const std::string &service,
                              std::string *clean_channel);

  static bool IsValidService(const std::string &service);

  bool Save();

  bool SaveNew();

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  uint64_t id_;
  std::string channel_;
  std::string service_;
  std::string overrustle_id_;
  std::string thumbnail_;
  bool live_;
  bool nsfw_;
  std::string path_;
  uint64_t viewer_count_{0};
  uint64_t rustler_count_{0};
  uint64_t reset_time_{0};
  uint64_t update_time_{0};
};

class Streams {
 public:
  explicit Streams(sqlite::database db_);

  void InitTable();

  std::vector<std::shared_ptr<Stream>> GetAllUpdatedSince(uint64_t timestamp);

  std::vector<std::shared_ptr<Stream>> GetAllWithRustlers();

  std::vector<std::shared_ptr<Stream>> GetAllWithRustlersSorted();

  std::string GetAPIJSON();

  void WriteStreamsJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

  inline std::shared_ptr<Stream> GetByID(const uint64_t id) {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    auto i = data_by_id_.find(id);
    return i == data_by_id_.end() ? nullptr : i->second;
  }

  inline std::shared_ptr<Stream> GetByChannel(const std::string &channel,
                                              const std::string &service) {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    auto i = data_by_channel_.find(std::make_tuple(channel, service));
    return i == data_by_channel_.end() ? nullptr : i->second;
  }

  std::shared_ptr<Stream> Emplace(const std::string &channel,
                                  const std::string &service,
                                  const std::string &overrustle_id);

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::unordered_map<uint64_t, std::shared_ptr<Stream>> data_by_id_;
  std::unordered_map<Channel, std::shared_ptr<Stream>, ChannelHash,
                     ChannelEqual>
      data_by_channel_;
};

}  // rustla2
