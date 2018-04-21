#pragma once

#include <glog/logging.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <sqlite_modern_cpp.h>
#include <algorithm>
#include <boost/thread/shared_mutex.hpp>

#include "Channel.h"
#include "Status.h"

namespace rustla2 {

// Cap stream IDs to 48 bits for JS
const uint64_t kMaxStreamID = 0xFFFFFFFFFFF;

class Stream {
 public:
  Stream(sqlite::database db, const uint64_t id, const Channel &channel,
         const std::string &thumbnail = "", const bool live = false,
         const uint64_t viewer_count = 0)
      : db_(db),
        id_(id),
        channel_(std::shared_ptr<Channel>(channel)),
        thumbnail_(thumbnail),
        live_(live),
        nsfw_(false),
        viewer_count_(viewer_count) {}

  Stream(sqlite::database db, const Channel &channel)
      : Stream(db, ChannelHash{}(channel)&kMaxStreamID, channel) {}

  inline uint64_t GetID() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return id_;
  }

  inline std::shared_ptr<Channel> GetChannel() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return channel_;
  }

  inline std::string GetThumbnail() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return thumbnail_;
  }

  inline bool GetLive() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return live_;
  }

  inline bool GetNSFW() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return nsfw_;
  }

  inline uint64_t GetViewerCount() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return viewer_count_;
  }

  inline uint64_t GetRustlerCount() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return rustler_count_;
  }

  inline uint64_t GetUpdateTime() const {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return update_time_;
  }

  inline uint64_t GetResetTime() const {
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

  inline bool SetChannel(std::shared_ptr<Channel> channel) {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    channel_ = channel;
    return true;
  }

  inline bool SetLive(const bool live) {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    live_ = live;
    return true;
  }

  inline bool SetThumbnail(const std::string &thumbnail) {
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

  bool Save();

  bool SaveNew();

  template <class T>
  bool Test(T filter) const {
    return filter.Test(*this);
  }

  template <class T, class... Ts>
  bool Test(T filter, Ts... filters) const {
    return filter.Test(*this) && Test(filters...);
  }

 private:
  sqlite::database db_;
  mutable boost::shared_mutex lock_;
  uint64_t id_;
  std::shared_ptr<Channel> channel_;
  std::string thumbnail_;
  bool live_;
  bool nsfw_;
  uint64_t viewer_count_{0};
  uint64_t rustler_count_{0};
  uint64_t reset_time_{0};
  uint64_t update_time_{0};
};

class UpdatedSince {
 public:
  explicit UpdatedSince(uint64_t timestamp) : timestamp_(timestamp) {}

  bool Test(const Stream &stream) const {
    return stream.GetUpdateTime() >= timestamp_;
  }

 private:
  uint64_t timestamp_;
};

struct HasRustlers {
  bool Test(const Stream &stream) const { return stream.GetRustlerCount() > 0; }
};

struct IsLive {
  bool Test(const Stream &stream) const { return stream.GetLive(); }
};

class Streams {
 public:
  explicit Streams(sqlite::database db_);

  void InitTable();

  template <class... Ts>
  std::vector<std::shared_ptr<Stream>> GetAllFiltered(Ts... filters) {
    std::vector<std::shared_ptr<Stream>> streams;

    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    for (const auto i : data_by_id_) {
      if (i.second->Test(filters...)) {
        streams.push_back(i.second);
      }
    }

    return streams;
  }

  template <class... Ts>
  std::vector<std::shared_ptr<Stream>> GetAllFilteredSorted(Ts... filters) {
    auto streams = GetAllFiltered(filters...);

    std::sort(streams.begin(), streams.end(),
              [](std::shared_ptr<Stream> a, std::shared_ptr<Stream> b) {
                return b->GetRustlerCount() < a->GetRustlerCount();
              });

    return streams;
  }

  std::vector<std::shared_ptr<Stream>> GetAllUpdatedSince(uint64_t timestamp);

  std::vector<std::shared_ptr<Stream>> GetAllWithRustlers();

  std::string GetAPIJSON();

  void WriteStreamsJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

  inline std::shared_ptr<Stream> GetByID(const uint64_t id) {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    auto it = data_by_id_.find(id);
    return it == data_by_id_.end() ? nullptr : it->second;
  }

  inline std::shared_ptr<Stream> GetByChannel(const Channel &channel) {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    auto it = data_by_channel_.find(channel);
    return it == data_by_channel_.end() ? nullptr : it->second;
  }

  std::shared_ptr<Stream> Emplace(const Channel &channel);

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::unordered_map<uint64_t, std::shared_ptr<Stream>> data_by_id_;
  std::unordered_map<Channel, std::shared_ptr<Stream>, ChannelHash,
                     ChannelEqual>
      data_by_channel_;
};

}  // namespace rustla2
