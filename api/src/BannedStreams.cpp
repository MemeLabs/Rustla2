#include "BannedStreams.h"

#include <glog/logging.h>
#include <boost/thread/locks.hpp>

namespace rustla2 {

BannedStreams::BannedStreams(sqlite::database db) : db_(db) {
  InitTable();

  db_ << "SELECT `channel`, `service` FROM `banned_streams`" >>
      [&](const std::string& channel, const std::string& service) {
        data_.insert(std::make_tuple(channel, service));
      };

  LOG(INFO) << "read " << data_.size() << " banned streams";
}

void BannedStreams::InitTable() {
  auto query = R"sql(
      CREATE TABLE IF NOT EXISTS `banned_streams` (
        `channel` VARCHAR(255) NOT NULL NOT NULL,
        `service` VARCHAR(255) NOT NULL NOT NULL,
        `reason` VARCHAR(255),
        `created_at` DATETIME NOT NULL,
        `updated_at` DATETIME NOT NULL,
        UNIQUE (`channel`, `service`),
        PRIMARY KEY (`channel`, `service`)
      )
    )sql";
  db_ << query;
}

bool BannedStreams::Emplace(const std::string& channel,
                            const std::string& service,
                            const std::string& reason) {
  try {
    auto sql = R"sql(
        INSERT INTO `banned_streams`
        VALUES (?, ?, ?, datetime(), datetime())
      )sql";
    db_ << sql << channel << service << reason;
  } catch (const sqlite::errors::error& e) {
    LOG(ERROR) << "error storing banned stream "
               << "channel: " << channel << ", "
               << "service: " << service << ", "
               << "reason: " << reason << ", "
               << "error: " << e.what();

    return false;
  }

  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  data_.insert(std::make_tuple(channel, service));

  return true;
}

}  // rustla2
