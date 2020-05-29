#include "IPRanges.h"

#include <folly/Conv.h>
#include <folly/Format.h>
#include <glog/logging.h>
#include <rapidjson/document.h>

#include <boost/asio/ip/address.hpp>
#include <limits>
#include <vector>

namespace rustla2 {

IPRanges::IPRanges(sqlite::database db, const std::string& table_name)
    : db_(db), table_name_(table_name) {
  InitTable();

  auto sql = folly::sformat("SELECT `start`, `end` FROM `{}`", table_name_);
  auto query = db_ << sql;

  query >> [&](const std::string start, const std::string end) {
    data_.insert(Value::closed(internal::GetAddressValue(start),
                               internal::GetAddressValue(end)));
  };

  LOG(INFO) << "read " << data_.size() << " ip ranges from " << table_name;
}

void IPRanges::InitTable() {
  auto sql = R"sql(
      CREATE TABLE IF NOT EXISTS `{}` (
        `start` VARCHAR(39),
        `end` VARCHAR(39),
        `note` VARCHAR(255),
        `created_at` DATETIME NOT NULL,
        `updated_at` DATETIME NOT NULL,
        UNIQUE (`start`, `end`)
      )
    )sql";
  db_ << folly::sformat(sql, table_name_);
}

bool IPRanges::Contains(const folly::StringPiece address_str) {
  const auto value = internal::GetAddressValue(address_str);
  if (value == 0) {
    return false;
  }

  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  return data_.find(value) != data_.end();
}

bool IPRanges::Insert(const std::string& range_start_str,
                      const std::string& range_end_str,
                      const std::string& note) {
  const auto range_start = internal::GetAddressValue(range_start_str);
  const auto range_end = internal::GetAddressValue(range_end_str);
  if (range_start == 0 || range_end == 0) {
    return false;
  }

  {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    if (data_.find(Value::closed(range_start, range_end)) != data_.end()) {
      return false;
    }

    data_.insert(Value::closed(range_start, range_end));
  }

  try {
    const auto sql = folly::sformat(
        "INSERT INTO `{}` VALUES (?, ?, ?, datetime(), datetime())",
        table_name_);

    db_ << sql << range_start_str << range_end_str << note;
  } catch (sqlite::errors::error& e) {
    LOG(ERROR) << "error storing ip range "
               << "start: " << range_start_str << ", "
               << "end: " << range_end_str << ", "
               << "note: " << note << ", "
               << "table: " << table_name_ << ", "
               << "error: " << e.what();

    return false;
  }

  return true;
}

uint32_t IPSet::Incr(const std::string& address_str) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  return ++counts_[address_str];
}

uint32_t IPSet::Decr(const std::string& address_str) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  const auto count = --counts_[address_str];
  if (count == 0) {
    counts_.erase(address_str);
  }
  return count;
}

namespace internal {

unsigned __int128 GetAddressValue(const folly::StringPiece address_str) {
  if (address_str.size() == 0) {
    return 0;
  }

  boost::system::error_code error;
  auto address =
      boost::asio::ip::address::from_string(address_str.toString(), error);
  if (error) {
    return 0;
  }

  if (address.is_v6()) {
    return PackAddressBytes(address.to_v6().to_bytes(), 16);
  } else if (address.is_v4()) {
    return PackAddressBytes(address.to_v4().to_bytes(), 4);
  }

  return 0;
}

}  // namespace internal
}  // namespace rustla2
