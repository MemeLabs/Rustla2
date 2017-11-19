#include "Users.h"

#include <glog/logging.h>

namespace rustla2 {

std::string User::GetStreamJSON() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  writer.StartObject();
  writer.Key("service");
  writer.String(channel_->GetService());
  writer.Key("channel");
  writer.String(channel_->GetChannel());
  writer.EndObject();

  return buf.GetString();
}

std::string User::GetProfileJSON() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  writer.StartObject();
  writer.Key("username");
  writer.String(id_);
  writer.Key("service");
  writer.String(channel_->GetService());
  writer.Key("channel");
  writer.String(channel_->GetChannel());
  writer.Key("left_chat");
  writer.Bool(left_chat_);
  writer.EndObject();

  return buf.GetString();
}

void User::WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  writer->StartObject();
  writer->Key("username");
  writer->String(id_);
  writer->Key("channel");
  channel_->WriteJSON(writer);
  writer->Key("left_chat");
  writer->Bool(left_chat_);
  writer->Key("last-ip");
  writer->String(last_ip_);
  writer->Key("last_seen");
  writer->Int(last_seen_);
  writer->Key("is_admin");
  writer->Bool(is_admin_);
  writer->EndObject();
}

bool User::SetChannel(const Channel &channel) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  channel_ = std::make_shared<Channel>(channel);
  return true;
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
    db_ << sql << channel_->GetService() << channel_->GetChannel() << last_ip_
        << last_seen_ << left_chat_ << is_admin_ << id_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error updating user "
               << "id: " << id_ << ", "
               << "service: " << channel_->GetService() << ", "
               << "channel: " << channel_->GetChannel() << ", "
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
    db_ << sql << id_ << channel_->GetService() << channel_->GetChannel()
        << last_ip_ << last_seen_ << left_chat_ << is_admin_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error creating user "
               << "id: " << id_ << ", "
               << "service: " << channel_->GetService() << ", "
               << "channel: " << channel_->GetChannel() << ", "
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
        strftime('%s', `last_seen`),
        `left_chat`,
        `is_admin`
      FROM `users`
    )sql";
  auto query = db_ << sql;

  query >> [&](const std::string &id, const std::string &service,
               const std::string &channel, const std::string &last_ip,
               const time_t last_seen, const bool left_chat,
               const bool is_admin) {
    auto stream_channel = Channel::Create(channel, service);
    data_[id] = std::make_shared<User>(db_, id, stream_channel, last_ip,
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
                                     const Channel &channel,
                                     const std::string &ip) {
  auto user = std::make_shared<User>(db_, name, channel, ip);

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

void Users::WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  writer->StartArray();
  for (const auto &it : data_) {
    it.second->WriteJSON(writer);
  }
  writer->EndArray();
}

}  // namespace rustla2
