#pragma once

#include <sqlite_modern_cpp.h>
#include <boost/thread/shared_mutex.hpp>
#include <chrono>
#include <memory>
#include <unordered_set>

#include "Channel.h"

namespace rustla2 {

class BannedStreams {
 public:
  explicit BannedStreams(sqlite::database db);

  void InitTable();

  inline bool Contains(const Channel& channel) {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return data_.count(channel);
  }

  bool Emplace(const Channel& channel, const std::string& reason = "");

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::unordered_set<Channel, ChannelSourceHash, ChannelSourceEqual> data_;
};

}  // namespace rustla2
