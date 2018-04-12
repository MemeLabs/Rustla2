#include "Users.h"

#include <glog/logging.h>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include "Config.h"
#include "Strings.h"

namespace rustla2 {

std::string User::GetStreamJSON() {
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

std::string User::GetUsernameJSON() {
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  writer.StartObject();
  writer.Key("username");
  writer.String(name_);
  writer.EndObject();

  return buf.GetString();
}

std::string User::GetProfileJSON() {
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  writer.StartObject();
  writer.Key("username");
  writer.String(name_);
  writer.Key("stream_path");
  writer.String(stream_path_);
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
  writer->StartObject();
  writer->Key("username");
  writer->String(name_);
  writer->Key("stream_path");
  writer->String(stream_path_);
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

Status User::SetStreamPath(const std::string &stream_path) {
  const boost::regex valid_path_regex("^[a-z0-9_]{3,32}$");
  if (!boost::regex_match(stream_path, valid_path_regex)) {
    return Status(StatusCode::VALIDATION_ERROR,
                  "Username may only contain a-z 0-9 or underscores and must "
                  "be betwee 3 and 32 characters in length.");
  }

  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  stream_path_ = stream_path;
  return Status::OK;
}

Status User::SetName(const std::string &name) {
  if (name_ != "") {
    if (name_ == name) {
      return Status::OK;
    }
    return Status(StatusCode::VALIDATION_ERROR, "Username cannot be changed.");
  }

  // based on destinygg/website username validation
  // https://github.com/destinygg/website/blob/0e984436d2d381f02666272e8bf38eb9ebda476a/lib/Destiny/Common/Authentication/AuthenticationCredentials.php

  const boost::regex valid_name_regex("^[A-Za-z0-9_]{3,20}$");
  if (!boost::regex_match(name, valid_name_regex)) {
    return Status(StatusCode::VALIDATION_ERROR,
                  "Username may only contain A-z 0-9 or underscores and must "
                  "be betwee 3 and 20 characters in length.");
  }

  std::string name_norm = name;
  folly::toLowerAscii(name_norm);
  auto name_start = name_norm.substr(0, 2);

  for (const auto &emote : Config::Get().GetEmotes()) {
    std::string emote_norm = emote;
    folly::toLowerAscii(emote_norm);

    if (folly::StringPiece(name_norm).contains(emote_norm)) {
      return Status(StatusCode::VALIDATION_ERROR,
                    "Username cannot contain emote.", "Contains " + emote);
    }

    if (emote.size() < 4) {
      continue;
    }

    auto emote_start = emote_norm.substr(0, 2);
    auto name_trunc = name_norm.substr(0, std::min(emote.size(), name.size()));

    if (emote_start == name_start &&
        levenshtein_distance(emote_norm, name_trunc) <= 2) {
      return Status(StatusCode::VALIDATION_ERROR,
                    "Username too similar to an emote, try changing the first "
                    "characters.");
    }
  }

  const boost::regex repeated_number_regex("[0-9]{3}");
  if (boost::regex_match(name, repeated_number_regex)) {
    return Status(StatusCode::VALIDATION_ERROR,
                  "Too many numbers in a row in username.");
  }

  const boost::regex repeated_underscore_regex("[_]{2}|(_.*_.*_)");
  if (boost::regex_match(name, repeated_underscore_regex)) {
    return Status(StatusCode::VALIDATION_ERROR,
                  "Too many underscores in username.");
  }

  const boost::regex non_numbers_regex("[^0-9]");
  auto name_numbers = boost::regex_replace(name, non_numbers_regex, "");
  if (name_numbers.size() > (name.size() + 1) / 2) {
    return Status(StatusCode::VALIDATION_ERROR,
                  "Number ratio is too high in username.");
  }

  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  name_ = name;
  return Status::OK;
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
          `name` = ?,
          `stream_path` = ?,
          `service` = ?,
          `channel` = ?,
          `last_ip` = ?,
          `last_seen` = datetime(?, 'unixepoch'),
          `left_chat` = ?,
          `is_admin` = ?,
          `updated_at` = datetime()
        WHERE `id` = ?
      )sql";
    db_ << sql << name_ << stream_path_ << channel_->GetService()
        << channel_->GetChannel() << last_ip_ << last_seen_ << left_chat_
        << is_admin_ << GetIDString();
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error updating user " << this << ", "
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
          `twitch_id`,
          `name`,
          `stream_path`,
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
    db_ << sql << GetIDString() << twitch_id_ << name_ << stream_path_
        << channel_->GetService() << channel_->GetChannel() << last_ip_
        << last_seen_ << left_chat_ << is_admin_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error creating user " << this << ", "
               << "error: " << e.what() << ", "
               << "code: " << e.get_extended_code();

    return false;
  }
  return true;
}

std::ostream &operator<<(std::ostream &os, const User &user) {
  os << "id: " << user.id_ << ", "
     << "twitch_id: " << user.twitch_id_ << ", "
     << "name: " << user.name_ << ", "
     << "stream_path: " << user.stream_path_ << ", "
     << "channel: " << user.channel_ << ", "
     << "last_ip: " << user.last_ip_ << ", "
     << "last_seen: " << user.last_seen_ << ", "
     << "left_chat: " << user.left_chat_ << ", "
     << "is_admin: " << user.is_admin_;
  return os;
}

Users::Users(sqlite::database db) : db_(db) {
  InitTable();

  auto sql = R"sql(
      SELECT
        `id`,
        `twitch_id`,
        `name`,
        `stream_path`,
        `service`,
        `channel`,
        `last_ip`,
        strftime('%s', `last_seen`),
        `left_chat`,
        `is_admin`
      FROM `users`
    )sql";
  auto query = db_ << sql;

  query >> [&](const std::string &id, const uint64_t twitch_id,
               const std::string &name, const std::string &stream_path,
               const std::string &service, const std::string &channel,
               const std::string &last_ip, const time_t last_seen,
               const bool left_chat, const bool is_admin) {
    boost::uuids::string_generator to_uuid;
    auto stream_channel = Channel::Create(channel, service);
    auto user = std::make_shared<User>(db_, to_uuid(id), twitch_id, name,
                                       stream_path, stream_channel, last_ip,
                                       last_seen, left_chat, is_admin);

    data_by_id_[user->GetID()] = user;
    data_by_twitch_id_[user->GetTwitchID()] = user;
    data_by_name_[user->GetName()] = user;
    data_by_stream_path_[user->GetStreamPath()] = user;
  };

  LOG(INFO) << "read " << data_by_id_.size() << " users";
}

void Users::InitTable() {
  auto sql = R"sql(
      CREATE TABLE IF NOT EXISTS `users` (
        `id` CHAR(36) NOT NULL,
        `twitch_id` UNSIGNED BIGINT NOT NULL,
        `name` VARCHAR(32) NOT NULL,
        `stream_path` VARCHAR(255) NOT NULL,
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
        UNIQUE (`id`),
        UNIQUE (`twitch_id`)
      );
    )sql";
  db_ << sql;
}

std::shared_ptr<User> Users::GetByID(const boost::uuids::uuid &id) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  auto i = data_by_id_.find(id);
  if (i == data_by_id_.end()) {
    return nullptr;
  }
  return i->second;
}

std::shared_ptr<User> Users::GetByTwitchID(const uint64_t twitch_id) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  auto i = data_by_twitch_id_.find(twitch_id);
  if (i == data_by_twitch_id_.end()) {
    return nullptr;
  }
  return i->second;
}

std::shared_ptr<User> Users::GetByName(const std::string &name) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  auto i = data_by_name_.find(name);
  if (i == data_by_name_.end()) {
    return nullptr;
  }
  return i->second;
}

std::shared_ptr<User> Users::GetByStreamPath(const std::string &stream_path) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  auto i = data_by_stream_path_.find(stream_path);
  if (i == data_by_stream_path_.end()) {
    return nullptr;
  }
  return i->second;
}

std::shared_ptr<User> Users::Emplace(const uint64_t twitch_id,
                                     const Channel &channel,
                                     const std::string &ip) {
  auto user = std::make_shared<User>(db_, twitch_id, channel, ip);

  {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    auto it = data_by_twitch_id_.find(user->GetTwitchID());
    if (it != data_by_twitch_id_.end()) {
      return it->second;
    }

    data_by_id_[user->GetID()] = user;
    data_by_twitch_id_[user->GetTwitchID()] = user;
  }

  bool saved = user->SaveNew();

  if (!saved) {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);

    data_by_id_.erase(user->GetID());
    data_by_twitch_id_.erase(user->GetTwitchID());

    return nullptr;
  }

  return user;
}

Status Users::Save(std::shared_ptr<User> user) {
  auto oldUser = GetByID(user->GetID());

  if (oldUser == nullptr) {
    return Status(StatusCode::VALIDATION_ERROR, "No user found.");
  }

  bool stream_path_changed = user->GetStreamPath() != oldUser->GetStreamPath();
  bool name_changed = user->GetName() != oldUser->GetName();

  // optimistically acquire new path and name if they're available
  {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);

    if (name_changed) {
      if (data_by_name_.count(user->GetName())) {
        return Status(StatusCode::VALIDATION_ERROR, "Name unavailable.");
      }
      data_by_name_[user->GetName()] = user;
    }

    if (stream_path_changed && !user->GetStreamPath().empty()) {
      if (data_by_stream_path_.count(user->GetStreamPath())) {
        return Status(StatusCode::VALIDATION_ERROR, "Stream path unavailable.");
      }
      data_by_stream_path_[user->GetStreamPath()] = user;
    }
  }

  bool saved = user->Save();

  boost::unique_lock<boost::shared_mutex> write_lock(lock_);

  if (saved) {
    data_by_id_[user->GetID()] = user;
    data_by_twitch_id_[user->GetTwitchID()] = user;

    if (name_changed && !oldUser->GetName().empty()) {
      data_by_name_.erase(oldUser->GetName());
    }
    if (stream_path_changed && !oldUser->GetStreamPath().empty()) {
      data_by_stream_path_.erase(oldUser->GetStreamPath());
    }

    return Status::OK;
  }

  if (name_changed && !user->GetName().empty()) {
    data_by_name_.erase(user->GetName());
  }
  if (stream_path_changed && !user->GetStreamPath().empty()) {
    data_by_stream_path_.erase(user->GetStreamPath());
  }

  return Status(StatusCode::DB_ENGINE_ERROR, "Error saving changes.");
}

void Users::WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  writer->StartArray();
  for (const auto &it : data_by_id_) {
    it.second->WriteJSON(writer);
  }
  writer->EndArray();
}

}  // namespace rustla2
