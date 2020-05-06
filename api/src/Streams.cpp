#include "Streams.h"

#include <algorithm>

namespace rustla2 {

void Stream::WriteAPIJSON(
    rapidjson::Writer<rapidjson::StringBuffer> *writer) const {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);

  writer->StartObject();
  writer->Key("live");
  writer->Bool(live_);
  writer->Key("nsfw");
  writer->Bool(nsfw_ || service_nsfw_);
  writer->Key("hidden");
  writer->Bool(hidden_);
  writer->Key("afk");
  writer->Bool(afk_);
  writer->Key("promoted");
  writer->Bool(promoted_);
  writer->Key("rustlers");
  writer->Uint64(rustler_count_ - afk_count_);
  writer->Key("afk_rustlers");
  writer->Uint64(afk_count_);
  writer->Key("service");
  writer->String(channel_->GetService());
  writer->Key("channel");
  writer->String(channel_->GetChannel());
  writer->Key("title");
  writer->String(title_);
  writer->Key("thumbnail");
  writer->String(thumbnail_);
  writer->Key("url");
  writer->String(channel_->GetPath());
  writer->Key("viewers");
  writer->Uint64(viewer_count_);
  writer->EndObject();
}

void Stream::WriteJSON(
    rapidjson::Writer<rapidjson::StringBuffer> *writer) const {
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
  writer->Key("title");
  writer->String(title_);
  writer->Key("thumbnail");
  writer->String(thumbnail_);
  writer->Key("live");
  writer->Bool(live_);
  writer->Key("nsfw");
  writer->Bool(nsfw_ || service_nsfw_);
  writer->Key("hidden");
  writer->Bool(hidden_);
  writer->Key("afk");
  writer->Bool(afk_);
  writer->Key("promoted");
  writer->Bool(promoted_);
  writer->Key("viewers");
  writer->Uint64(viewer_count_);
  writer->Key("rustlers");
  writer->Uint64(rustler_count_ - afk_count_);
  writer->Key("afk_rustlers");
  writer->Uint64(afk_count_);

  writer->EndObject();
}

uint64_t Stream::IncrRustlerCount() {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  observers_->Mark(id_);

  ResetUpdatedTime();
  if (rustler_count_ == afk_count_) {
    reset_time_ = update_time_;
  }

  return ++rustler_count_;
}

uint64_t Stream::DecrRustlerCount(const bool decr_afk) {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  observers_->Mark(id_);

  if (rustler_count_ == 0) {
    LOG(WARNING) << "DecrRustlerCount called on stream with 0 rustlers";
    return 0;
  }

  ResetUpdatedTime();
  if (decr_afk) {
    --afk_count_;
  }
  return --rustler_count_;
}

uint64_t Stream::IncrAFKCount() {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  observers_->Mark(id_);

  if (afk_count_ == rustler_count_) {
    LOG(WARNING) << "IncrAFKCount called on stream with all afks";
    return afk_count_;
  }

  ResetUpdatedTime();
  return ++afk_count_;
}

uint64_t Stream::DecrAFKCount() {
  boost::unique_lock<boost::shared_mutex> write_lock(lock_);
  observers_->Mark(id_);

  if (afk_count_ == 0) {
    LOG(WARNING) << "DecrAFKCount called on stream with 0 afks";
    return 0;
  }

  ResetUpdatedTime();
  if (rustler_count_ == afk_count_) {
    reset_time_ = update_time_;
  }
  return --afk_count_;
}

bool Stream::Save() {
  boost::shared_lock<boost::shared_mutex> read_lock(lock_);
  try {
    const auto sql = R"sql(
        UPDATE `streams` SET
          `channel` = ?,
          `service` = ?,
          `path` = ?,
          `nsfw` = ?,
          `hidden` = ?,
          `afk` = ?,
          `promoted` = ?,
          `title` = ?,
          `thumbnail` = ?,
          `live` = ?,
          `viewers` = ?,
          `service_nsfw` = ?,
          `updated_at` = datetime()
        WHERE `id` = ?
      )sql";
    db_ << sql << channel_->GetChannel() << channel_->GetService()
        << channel_->GetStreamPath() << nsfw_ << hidden_ << afk_ << promoted_
        << title_ << thumbnail_ << live_ << viewer_count_ << service_nsfw_
        << id_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error updating stream " << this << ", "
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
          `nsfw`,
          `hidden`,
          `afk`,
          `promoted`,
          `title`,
          `thumbnail`,
          `live`,
          `viewers`,
          `service_nsfw`,
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
        << channel_->GetStreamPath() << nsfw_ << hidden_ << afk_ << promoted_
        << title_ << thumbnail_ << live_ << viewer_count_ << service_nsfw_;
  } catch (const sqlite::sqlite_exception &e) {
    LOG(ERROR) << "error creating stream " << this << ", "
               << "error: " << e.what() << ", "
               << "code: " << e.get_extended_code();

    return false;
  }

  return true;
}

Streams::Streams(sqlite::database db)
    : db_(db), observers_(std::make_shared<Observable<uint64_t>>()) {
  InitTable();

  auto sql = R"sql(
      SELECT
        `id`,
        `channel`,
        `service`,
        `path`,
        `nsfw`,
        `hidden`,
        `afk`,
        `promoted`,
        `title`,
        `thumbnail`,
        `live`,
        `viewers`,
        `service_nsfw`
      FROM `streams`
    )sql";
  auto query = db_ << sql;

  query >> [&](const uint64_t id, const std::string &channel,
               const std::string &service, const std::string &path, bool nsfw,
               bool hidden, bool afk, bool promoted, const std::string &title,
               const std::string &thumbnail, const bool live,
               const uint64_t viewer_count, const bool service_nsfw) {
    auto stream_channel = Channel::Create(channel, service, path);
    auto stream = std::make_shared<Stream>(
        db_, observers_, id, stream_channel, nsfw, hidden, afk, promoted, title,
        thumbnail, live, viewer_count, service_nsfw);

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
        `nsfw` TINYINT(1) DEFAULT 0,
        `hidden` TINYINT(1) DEFAULT 0,
        `afk` TINYINT(1) DEFAULT 0,
        `promoted` TINYINT(1) DEFAULT 0,
        `title` VARCHAR(255) NOT NULL,
        `thumbnail` VARCHAR(255),
        `live` TINYINT(1) DEFAULT 0,
        `viewers` INTEGER DEFAULT 0,
        `service_nsfw` TINYINT(1) DEFAULT 0,
        `created_at` DATETIME NOT NULL,
        `updated_at` DATETIME NOT NULL,
        UNIQUE (`id`),
        UNIQUE (`channel`, `service`, `path`)
      )
    )sql";
  db_ << sql;
}

std::ostream &operator<<(std::ostream &os, const Stream &stream) {
  os << "id " << stream.id_ << ", "
     << "channel " << stream.channel_->GetChannel() << ", "
     << "service " << stream.channel_->GetService() << ", "
     << "path " << stream.channel_->GetStreamPath() << ", "
     << "nsfw " << stream.nsfw_ << ", "
     << "hidden " << stream.hidden_ << ", "
     << "afk " << stream.afk_ << ", "
     << "promoted " << stream.promoted_ << ", "
     << "title " << stream.title_ << ", "
     << "thumbnail " << stream.thumbnail_ << ", "
     << "live " << stream.live_ << ", "
     << "viewer_count " << stream.viewer_count_ << ", ";
  return os;
}

std::vector<std::shared_ptr<Stream>>
Streams::GetAllUpdatedSince(uint64_t timestamp) {
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
  auto stream = std::make_shared<Stream>(db_, observers_, channel);

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

} // namespace rustla2
