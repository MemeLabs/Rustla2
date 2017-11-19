#include "BannedStreams.h"

#include <glog/logging.h>

namespace rustla2 {

BannedStreams::BannedStreams(sqlite::database db) : db_(db) {
  InitTable();

  db_ << "SELECT `channel`, `service` FROM `banned_streams`" >>
      [&](const std::string& channel, const std::string& service) {
        data_.insert(Channel::Create(channel, service));
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

bool BannedStreams::Emplace(const Channel& channel, const std::string& reason) {
  try {
    auto sql = R"sql(
        INSERT INTO `banned_streams`
        VALUES (?, ?, ?, datetime(), datetime())
      )sql";
    db_ << sql << channel.GetChannel() << channel.GetService() << reason;
  } catch (const sqlite::errors::error& e) {
    LOG(ERROR) << "error storing banned stream "
               << "channel: " << channel.GetChannel() << ", "
               << "service: " << channel.GetService() << ", "
               << "reason: " << reason << ", "
               << "error: " << e.what();

    return false;
  }

  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  data_.insert(channel);

  return true;
}

}  // namespace rustla2
