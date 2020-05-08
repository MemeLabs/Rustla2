#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sqlite_modern_cpp.h>

#include "../src/Observer.h"
#include "../src/Streams.h"
#include "rapidjson/document.h"

namespace rustla2 {

TEST(StreamsTest, TestWriteAPIJSONDefault) {
  sqlite::database db(":memory:");

  auto streams = new Streams(db);
  auto status = Status(StatusCode::OK, "");
  auto chn = Channel::Create("test", "twitch", &status);
  auto stream = streams->Emplace(chn);

  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  stream->WriteAPIJSON(&writer);

  rapidjson::Document doc;
  doc.Parse(buf.GetString());

  EXPECT_FALSE(doc["nsfw"].GetBool());
  EXPECT_FALSE(doc["live"].GetBool());
  EXPECT_FALSE(doc["hidden"].GetBool());
  EXPECT_FALSE(doc["afk"].GetBool());
  EXPECT_FALSE(doc["promoted"].GetBool());
  EXPECT_EQ(doc["rustlers"].GetInt(), 0);
  EXPECT_EQ(doc["afk_rustlers"].GetInt(), 0);
  EXPECT_STREQ(doc["service"].GetString(), "twitch");
  EXPECT_STREQ(doc["channel"].GetString(), "test");
  EXPECT_STREQ(doc["thumbnail"].GetString(), "");
  EXPECT_STREQ(doc["url"].GetString(), "/twitch/test");
}

TEST(StreamsTest, TestWriteAPIJSONToggleBooleans) {
  sqlite::database db(":memory:");

  auto streams = new Streams(db);
  auto status = Status(StatusCode::OK, "");
  auto chn = Channel::Create("test", "twitch", &status);
  auto stream = streams->Emplace(chn);

  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

  stream->SetNSFW(true);
  stream->SetAFK(true);
  stream->SetHidden(true);
  stream->SetPromoted(true);
  stream->SetLive(true);
  stream->WriteAPIJSON(&writer);

  rapidjson::Document doc;
  doc.Parse(buf.GetString());

  EXPECT_TRUE(doc["nsfw"].GetBool());
  EXPECT_TRUE(doc["live"].GetBool());
  EXPECT_TRUE(doc["hidden"].GetBool());
  EXPECT_TRUE(doc["afk"].GetBool());
  EXPECT_TRUE(doc["promoted"].GetBool());
  EXPECT_EQ(doc["rustlers"].GetInt(), 0);
  EXPECT_EQ(doc["afk_rustlers"].GetInt(), 0);
  EXPECT_STREQ(doc["service"].GetString(), "twitch");
  EXPECT_STREQ(doc["channel"].GetString(), "test");
  EXPECT_STREQ(doc["thumbnail"].GetString(), "");
  EXPECT_STREQ(doc["url"].GetString(), "/twitch/test");
}

TEST(StreamsTest, TestGetAPIJSON) {
  sqlite::database db(":memory:");

  auto streams = new Streams(db);
  auto status = Status(StatusCode::OK, "");
  auto valid = streams->Emplace(Channel::Create("test", "youtube", &status));
  auto removed = streams->Emplace(Channel::Create("jbpratt", "mixer", &status));
  auto notlive = streams->Emplace(Channel::Create("jbpratt", "twitch", &status));

  valid->SetLive(true);
  valid->IncrRustlerCount();
  removed->IncrRustlerCount();
  removed->SetLive(true);
  removed->SetRemoved(true);

  auto json = streams->GetAPIJSON();
  rapidjson::Document doc;
  doc.Parse(json);

  EXPECT_EQ(doc["stream_list"].GetArray().Size(), 1);
  EXPECT_EQ(doc["streams"].GetObject()["/youtube/test"].GetUint64(), 1);
}
} // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
