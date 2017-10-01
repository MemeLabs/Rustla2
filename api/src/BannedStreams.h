#pragma once

#include <sqlite_modern_cpp.h>
#include <boost/thread/shared_mutex.hpp>
#include <chrono>
#include <memory>
#include <unordered_set>

#include "Streams.h"

namespace rustla2 {

class BannedStream {
 private:
  std::string channel_;
  std::string service_;
};

class BannedStreams {
 public:
  explicit BannedStreams(sqlite::database db);

  void InitTable();

  inline bool Contains(const std::string& channel, const std::string& service) {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return data_.count(std::make_tuple(channel, service));
  }

  bool Emplace(const std::string& channel, const std::string& service,
               const std::string& reason = "");

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::unordered_set<Channel, ChannelHash, ChannelEqual> data_;
};

}  // rustla2
