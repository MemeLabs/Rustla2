#include "Streams.h"

#include <folly/Uri.h>
#include <glog/logging.h>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/thread/locks.hpp>
#include <vector>

namespace rustla2 {

namespace {
const boost::regex valid_channel_regex("^[a-zA-Z0-9\\-_]{1,64}$");
}

void Stream::WriteAPIJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  writer->StartObject();
  writer->Key("live");
  writer->Bool(live_);
  writer->Key("nsfw");
  writer->Bool(nsfw_);
  writer->Key("rustlers");
  writer->Uint64(rustler_count_);
  writer->Key("service");
  writer->String(service_.c_str(), service_.size());
  writer->Key("thumbnail");
  writer->String(thumbnail_.c_str(), thumbnail_.size());
  writer->Key("url");
  writer->String(path_.c_str(), path_.size());
  writer->Key("viewers");
  writer->Uint64(viewer_count_);
  writer->EndObject();
}

void Stream::WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  writer->StartObject();
  writer->Key("id");
  writer->Uint64(id_);
  writer->Key("channel");
  writer->String(channel_.c_str(), channel_.size());
  writer->Key("service");
  writer->String(service_.c_str(), service_.size());
  writer->Key("overrustle_id");
  writer->String(overrustle_id_.c_str(), overrustle_id_.size());
  writer->Key("thumbnail");
  writer->String(thumbnail_.c_str(), thumbnail_.size());
  writer->Key("live");
  writer->Bool(live_);
  writer->Key("nsfw");
  writer->Bool(nsfw_);
  writer->Key("viewers");
  writer->Uint64(viewer_count_);
  writer->Key("rustlers");
  writer->Uint64(rustler_count_);

  writer->EndObject();
}

bool Stream::SetChannelAndService(const std::string &channel,
                                  const std::string &service) {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  std::string clean_channel;
  if (Stream::IsValidService(service) &&
      Stream::SanitizeChannel(channel, service, &clean_channel)) {
    service_ = service;
    channel_ = clean_channel;
    return true;
  }

  return false;
}

bool Stream::SanitizeChannel(const std::string &channel,
                             const std::string &service,
                             std::string *clean_channel) {
  if (service == kAdvancedService) {
    try {
      auto channel_uri = folly::Uri(channel);

      if (channel_uri.scheme() != "http" && channel_uri.scheme() != "https") {
        return false;
      }

      *clean_channel = channel_uri.str();
    } catch (const std::invalid_argument &e) {
      return false;
    }
  } else {
    if (!boost::regex_match(channel, valid_channel_regex)) {
      return false;
    }

    *clean_channel = channel;
  }

  return true;
}

bool Stream::IsValidService(const std::string &service) {
  auto service_it = std::find(kServices.begin(), kServices.end(), service);
  if (service_it == kServices.end()) {
    return false;
  }

  return true;
}

bool Stream::Save() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  try {
    const auto sql = R"sql(
        UPDATE `streams` SET
          `channel` = ?,
          `service` = ?,
          `overrustle_id` = ?,
          `thumbnail` = ?,
          `live` = ?,
          `viewers` = ?,
          `updated_at` = datetime()
        WHERE `id` = ?
      )sql";
    db_ << sql << channel_ << service_ << overrustle_id_ << thumbnail_ << live_
        << viewer_count_ << id_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error updating stream "
               << "id " << id_ << ", "
               << "channel " << channel_ << ", "
               << "service " << service_ << ", "
               << "overrustle_id " << overrustle_id_ << ", "
               << "thumbnail " << thumbnail_ << ", "
               << "live " << live_ << ", "
               << "viewer_count " << viewer_count_ << ", "
               << "error: " << e.what() << ", "
               << "code: " << e.get_extended_code();

    return false;
  }

  return true;
}

bool Stream::SaveNew() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  try {
    const auto sql = R"sql(
        INSERT INTO `streams` (
          `id`,
          `channel`,
          `service`,
          `overrustle_id`,
          `thumbnail`,
          `live`,
          `viewers`,
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
          datetime(),
          datetime()
        )
      )sql";
    db_ << sql << id_ << channel_ << service_ << overrustle_id_ << thumbnail_
        << live_ << viewer_count_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error creating stream "
               << "id " << id_ << ", "
               << "channel " << channel_ << ", "
               << "service " << service_ << ", "
               << "overrustle_id " << overrustle_id_ << ", "
               << "thumbnail " << thumbnail_ << ", "
               << "live " << live_ << ", "
               << "viewer_count " << viewer_count_ << ", "
               << "error: " << e.what() << ", "
               << "code: " << e.get_extended_code();

    return false;
  }

  return true;
}

Streams::Streams(sqlite::database db) : db_(db) {
  InitTable();

  auto sql = R"sql(
      SELECT
        `id`,
        `channel`,
        `service`,
        `overrustle_id`,
        `thumbnail`,
        `live`,
        `viewers`
      FROM `streams`
    )sql";
  auto query = db_ << sql;

  query >> [&](const uint64_t id, const std::string &channel,
               const std::string &service, const std::string &overrustle_id,
               const std::string &thumbnail, const bool live,
               const uint64_t viewer_count) {
    auto stream =
        std::make_shared<Stream>(db_, id, channel, service, overrustle_id,
                                 thumbnail, live, viewer_count);

    data_by_id_[stream->GetID()] = stream;
    data_by_channel_[std::make_tuple(channel, service)] = stream;
  };

  LOG(INFO) << "read " << data_by_id_.size() << " streams";
}

void Streams::InitTable() {
  auto sql = R"sql(
      CREATE TABLE IF NOT EXISTS `streams` (
        `id` INTEGER PRIMARY KEY,
        `channel` VARCHAR(255) NOT NULL,
        `service` VARCHAR(255) NOT NULL,
        `overrustle_id` VARCHAR(255) REFERENCES `users` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
        `thumbnail` VARCHAR(255),
        `live` TINYINT(1) DEFAULT 0,
        `viewers` INTEGER DEFAULT 0,
        `created_at` DATETIME NOT NULL,
        `updated_at` DATETIME NOT NULL,
        UNIQUE (`id`),
        UNIQUE (`channel`, `service`)
      );
    )sql";
  db_ << sql;
}

std::vector<std::shared_ptr<Stream>> Streams::GetAllUpdatedSince(
    uint64_t timestamp) {
  std::vector<std::shared_ptr<Stream>> streams;

  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  for (const auto i : data_by_id_) {
    if (i.second->GetUpdateTime() >= timestamp) {
      streams.push_back(i.second);
    }
  }

  return streams;
}

std::vector<std::shared_ptr<Stream>> Streams::GetAllWithRustlers() {
  std::vector<std::shared_ptr<Stream>> streams;

  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  for (const auto i : data_by_id_) {
    if (i.second->GetRustlerCount() > 0) {
      streams.push_back(i.second);
    }
  }

  return streams;
}

std::vector<std::shared_ptr<Stream>> Streams::GetAllWithRustlersSorted() {
  auto streams = GetAllWithRustlers();

  std::sort(streams.begin(), streams.end(),
            [](std::shared_ptr<Stream> a, std::shared_ptr<Stream> b) {
              return b->GetRustlerCount() < a->GetRustlerCount();
            });

  return streams;
}

std::string Streams::GetAPIJSON() {
  auto streams = GetAllWithRustlersSorted();

  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
  writer.StartObject();

  writer.Key("stream_list");
  writer.StartArray();
  for (const auto &stream : streams) {
    stream->WriteAPIJSON(&writer);
  }
  writer.EndArray();

  writer.Key("streams");
  writer.StartObject();
  for (const auto &stream : streams) {
    writer.Key(stream->GetPath().c_str());
    writer.Uint64(stream->GetRustlerCount());
  }
  writer.EndObject();

  writer.EndObject();

  return buf.GetString();
}

void Streams::WriteStreamsJSON(
    rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  auto streams = GetAllWithRustlersSorted();

  writer->StartArray();
  for (const auto &stream : streams) {
    stream->WriteJSON(writer);
  }
  writer->EndArray();
}

std::shared_ptr<Stream> Streams::Emplace(const std::string &channel,
                                         const std::string &service,
                                         const std::string &overrustle_id) {
  auto stream = std::make_shared<Stream>(db_, channel, service, overrustle_id);
  if (!stream->GetIsValid()) {
    return nullptr;
  }

  {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    auto it = data_by_id_.find(stream->GetID());
    if (it != data_by_id_.end()) {
      return it->second;
    }

    data_by_id_[stream->GetID()] = stream;
    data_by_channel_[std::make_tuple(channel, service)] = stream;
  }

  stream->SaveNew();

  return stream;
}

}  // rustla2
