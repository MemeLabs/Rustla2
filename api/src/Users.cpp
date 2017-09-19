#include "Users.h"

#include <glog/logging.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <boost/thread/locks.hpp>

#include "Streams.h"

namespace rustla2 {

std::string User::GetStreamJSON() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  writer.StartObject();
  writer.Key("service");
  writer.String(service_.c_str());
  writer.Key("channel");
  writer.String(channel_.c_str());
  writer.EndObject();

  return buf.GetString();
}

std::string User::GetProfileJSON() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  writer.StartObject();
  writer.Key("username");
  writer.String(id_.c_str());
  writer.Key("service");
  writer.String(service_.c_str());
  writer.Key("channel");
  writer.String(channel_.c_str());
  writer.Key("left_chat");
  writer.Bool(left_chat_);
  writer.EndObject();

  return buf.GetString();
}

bool User::SetChannelAndService(const std::string &channel,
                                const std::string &service) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);

  std::string clean_channel;
  if (Stream::IsValidService(service) &&
      Stream::SanitizeChannel(channel, service, &clean_channel)) {
    service_ = service;
    channel_ = clean_channel;
    return true;
  }

  return false;
}

bool User::SetLeftChat(bool left_chat) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  left_chat_ = left_chat;
  return true;
}

bool User::SetLastIP(const std::string &last_ip) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  last_ip_ = last_ip;
  return true;
}

bool User::SetLastSeen(const time_t last_seen) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  last_seen_ = last_seen;
  return true;
}

bool User::Save() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  try {
    const auto sql = R"sql(
        UPDATE `users` SET
          `service` = ?,
          `channel` = ?,
          `last_ip` = ?,
          `last_seen` = datetime(?, 'unixepoch'),
          `left_chat` = ?,
          `is_admin` = ?,
          `updated_at` = datetime()
        WHERE `id` = ?
      )sql";
    db_ << sql << service_ << channel_ << last_ip_ << last_seen_ << left_chat_
        << is_admin_ << id_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error updating user "
               << "id: " << id_ << ", "
               << "service: " << service_ << ", "
               << "channel: " << channel_ << ", "
               << "last_ip: " << last_ip_ << ", "
               << "last_seen: " << last_seen_ << ", "
               << "left_chat: " << left_chat_ << ", "
               << "is_admin: " << is_admin_ << ", "
               << "error: " << e.what() << ", "
               << "code: " << e.get_extended_code();

    return false;
  }
  return true;
}

bool User::SaveNew() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  try {
    const auto sql = R"sql(
        INSERT INTO `users` (
          `id`,
          `service`,
          `channel`,
          `last_ip`,
          `last_seen`,
          `left_chat`,
          `is_admin`,
          `ban_reason`,
          `created_at`,
          `updated_at`
        )
        VALUES (
          ?,
          ?,
          ?,
          ?,
          datetime(?, 'unixepoch'),
          ?,
          ?,
          '',
          datetime(),
          datetime()
        )
      )sql";
    db_ << sql << id_ << service_ << channel_ << last_ip_ << last_seen_
        << left_chat_ << is_admin_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error creating user "
               << "id: " << id_ << ", "
               << "service: " << service_ << ", "
               << "channel: " << channel_ << ", "
               << "last_ip: " << last_ip_ << ", "
               << "last_seen: " << last_seen_ << ", "
               << "left_chat: " << left_chat_ << ", "
               << "is_admin: " << is_admin_ << ", "
               << "error: " << e.what() << ", "
               << "code: " << e.get_extended_code();

    return false;
  }
  return true;
}

Users::Users(sqlite::database db) : db_(db) {
  InitTable();

  auto sql = R"sql(
      SELECT
        `id`,
        `service`,
        `channel`,
        `last_ip`,
        `last_seen`,
        `left_chat`,
        `is_admin`
      FROM `users`
    )sql";
  auto query = db_ << sql;

  query >> [&](const std::string &id, const std::string &service,
               const std::string &channel, const std::string &last_ip,
               const time_t last_seen, const bool left_chat,
               const bool is_admin) {
    data_[id] = std::make_shared<User>(db_, id, service, channel, last_ip,
                                       last_seen, left_chat, is_admin);
  };

  LOG(INFO) << "read " << data_.size() << " users";
}

void Users::InitTable() {
  auto sql = R"sql(
      CREATE TABLE IF NOT EXISTS `users` (
        `id` VARCHAR(255) NOT NULL UNIQUE PRIMARY KEY,
        `service` VARCHAR(255) NOT NULL,
        `channel` VARCHAR(255) NOT NULL,
        `last_ip` VARCHAR(255) NOT NULL,
        `last_seen` DATETIME NOT NULL,
        `left_chat` TINYINT(1) DEFAULT 0,
        `is_banned` TINYINT(1) NOT NULL DEFAULT 0,
        `ban_reason` VARCHAR(255),
        `created_at` DATETIME NOT NULL,
        `updated_at` DATETIME NOT NULL,
        `is_admin` TINYINT(1) DEFAULT 0,
        UNIQUE (`id`)
      );
    )sql";
  db_ << sql;
}

std::shared_ptr<User> Users::GetByName(const std::string &name) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  auto i = data_.find(name);
  if (i == data_.end()) {
    return nullptr;
  }
  return i->second;
}

std::shared_ptr<User> Users::Emplace(const std::string &name,
                                     const std::string &service,
                                     const std::string &channel,
                                     const std::string &ip) {
  auto user = std::make_shared<User>(db_, name, service, channel, ip);

  {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    auto it = data_.find(user->GetID());
    if (it != data_.end()) {
      return it->second;
    }

    data_[user->GetID()] = user;
  }

  user->SaveNew();

  return user;
}

}  // rustla2
