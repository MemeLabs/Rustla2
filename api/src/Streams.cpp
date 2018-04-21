#include "Streams.h"

#include <algorithm>

namespace rustla2 {

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
  writer->String(channel_->GetService());
  writer->Key("channel");
  writer->String(channel_->GetChannel());
  writer->Key("thumbnail");
  writer->String(thumbnail_);
  writer->Key("url");
  writer->String(channel_->GetPath());
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
  writer->String(channel_->GetChannel());
  writer->Key("service");
  writer->String(channel_->GetService());
  writer->Key("overrustle_id");
  writer->String(channel_->GetStreamPath());
  writer->Key("thumbnail");
  writer->String(thumbnail_);
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

bool Stream::Save() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  try {
    const auto sql = R"sql(
        UPDATE `streams` SET
          `channel` = ?,
          `service` = ?,
          `path` = ?,
          `thumbnail` = ?,
          `live` = ?,
          `viewers` = ?,
          `updated_at` = datetime()
        WHERE `id` = ?
      )sql";
    db_ << sql << channel_->GetChannel() << channel_->GetService()
        << channel_->GetStreamPath() << thumbnail_ << live_ << viewer_count_
        << id_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error updating stream "
               << "id " << id_ << ", "
               << "channel " << channel_->GetChannel() << ", "
               << "service " << channel_->GetService() << ", "
               << "path " << channel_->GetStreamPath() << ", "
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
          `path`,
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
    db_ << sql << id_ << channel_->GetChannel() << channel_->GetService()
        << channel_->GetStreamPath() << thumbnail_ << live_ << viewer_count_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error creating stream "
               << "id " << id_ << ", "
               << "channel " << channel_->GetChannel() << ", "
               << "service " << channel_->GetService() << ", "
               << "path " << channel_->GetStreamPath() << ", "
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
        `path`,
        `thumbnail`,
        `live`,
        `viewers`
      FROM `streams`
    )sql";
  auto query = db_ << sql;

  query >> [&](const uint64_t id, const std::string &channel,
               const std::string &service, const std::string &path,
               const std::string &thumbnail, const bool live,
               const uint64_t viewer_count) {
    auto stream_channel = Channel::Create(channel, service, path);
    auto stream = std::make_shared<Stream>(db_, id, stream_channel, thumbnail,
                                           live, viewer_count);

    data_by_id_[stream->GetID()] = stream;
    data_by_channel_[stream_channel] = stream;
  };

  LOG(INFO) << "read " << data_by_id_.size() << " streams";
}

void Streams::InitTable() {
  auto sql = R"sql(
      CREATE TABLE IF NOT EXISTS `streams` (
        `id` INTEGER PRIMARY KEY,
        `channel` VARCHAR(255) NOT NULL,
        `service` VARCHAR(255) NOT NULL,
        `path` VARCHAR(255) REFERENCES `users` (`stream_path`) ON DELETE SET NULL ON UPDATE CASCADE,
        `thumbnail` VARCHAR(255),
        `live` TINYINT(1) DEFAULT 0,
        `viewers` INTEGER DEFAULT 0,
        `created_at` DATETIME NOT NULL,
        `updated_at` DATETIME NOT NULL,
        UNIQUE (`id`),
        UNIQUE (`channel`, `service`, `path`)
      );
    )sql";
  db_ << sql;
}

std::vector<std::shared_ptr<Stream>> Streams::GetAllUpdatedSince(
    uint64_t timestamp) {
  return GetAllFiltered(UpdatedSince(timestamp));
}

std::vector<std::shared_ptr<Stream>> Streams::GetAllWithRustlers() {
  return GetAllFiltered(HasRustlers());
}

std::string Streams::GetAPIJSON() {
  auto streams = GetAllFilteredSorted(HasRustlers(), IsLive());

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
    writer.Key(stream->GetChannel()->GetPath().c_str());
    writer.Uint64(stream->GetRustlerCount());
  }
  writer.EndObject();

  writer.EndObject();

  return buf.GetString();
}

void Streams::WriteStreamsJSON(
    rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  auto streams = GetAllFilteredSorted(HasRustlers());

  writer->StartArray();
  for (const auto &stream : streams) {
    stream->WriteJSON(writer);
  }
  writer->EndArray();
}

std::shared_ptr<Stream> Streams::Emplace(const Channel &channel) {
  auto stream = std::make_shared<Stream>(db_, channel);

  {
    boost::unique_lock<boost::shared_mutex> write_lock(lock_);
    auto it = data_by_id_.find(stream->GetID());
    if (it != data_by_id_.end()) {
      return it->second;
    }

    data_by_id_[stream->GetID()] = stream;
    data_by_channel_[channel] = stream;
  }

  stream->SaveNew();

  return stream;
}

}  // namespace rustla2
