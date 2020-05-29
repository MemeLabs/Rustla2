#pragma once

#include <folly/String.h>
#include <glog/logging.h>
#include <sqlite_modern_cpp.h>

#include <algorithm>
#include <boost/asio/ip/address.hpp>
#include <boost/icl/interval_set.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <cstdint>
#include <cstring>

namespace rustla2 {

class IPRanges;

class IPSet {
 public:
  uint32_t Incr(const std::string& address_str);
  uint32_t Decr(const std::string& address_str);

 private:
  std::unordered_map<std::string, uint32_t> counts_;
  mutable boost::shared_mutex lock_;

  friend class IPRanges;
};

class IPRanges {
 public:
  using ValueRanges = boost::icl::interval_set<unsigned __int128>;
  using Value = ValueRanges::interval_type;

  IPRanges(sqlite::database db, const std::string& table_name);

  void InitTable();

  bool Contains(const folly::StringPiece address_str);

  bool Insert(const std::string& address_str) {
    return Insert(address_str, address_str);
  }

  bool Insert(std::shared_ptr<IPSet> ip_set, const std::string& note = "") {
    boost::shared_lock<boost::shared_mutex> read_lock(ip_set->lock_);
    for (auto it : ip_set->counts_) {
      Insert(it.first, it.first, note);
    }
    return true;
  }

  bool Insert(const std::string& range_start_str,
              const std::string& range_end_str, const std::string& note = "");

 private:
  sqlite::database db_;
  const std::string table_name_;
  boost::shared_mutex lock_;
  ValueRanges data_;
};

namespace internal {

unsigned __int128 GetAddressValue(const folly::StringPiece address_str);

template <typename T>
inline unsigned __int128 PackAddressBytes(T bytes, size_t length) {
  // prefix ipv4 addresses with 0xffff
  unsigned __int128 value = std::numeric_limits<uint16_t>::max();

  for (int i = 0; i < length; ++i) {
    value = (value << 8) | bytes[i];
  }
  return value;
}

}  // namespace internal
}  // namespace rustla2
