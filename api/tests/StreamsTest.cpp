#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sqlite_modern_cpp.h>

#include "../src/Observer.h"
#include "../src/Streams.h"
#include "../src/Users.h"
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
  auto removed = streams->Emplace(Channel::Create("jbpratt", "angelthump", &status));
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
rapidjson::Document StreamJSON(std::shared_ptr<Stream> stream, bool api = false) {
  rapidjson::StringBuffer buf;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
  if (api) stream->WriteAPIJSON(&writer);
  else stream->WriteJSON(&writer);
  rapidjson::Document doc;
  doc.Parse(buf.GetString());
  return doc;
}

TEST(StreamsTest, ProfileTitleUpdatesWithoutOverwritingProviderTitle) {
  sqlite::database db(":memory:");
  auto users = std::make_shared<Users>(db);
  Streams streams(db, users);
  auto channel = Channel::Create("test", "angelthump", "movies");
  auto user = users->Emplace(1, channel, "127.0.0.1");
  ASSERT_NE(user, nullptr);
  ASSERT_TRUE(user->SetStreamTitleOverride("Movie night").Ok());
  ASSERT_TRUE(users->Save(user).Ok());
  auto stream = streams.Emplace(channel);
  stream->SetTitle("Provider title");

  for (bool api : {false, true}) {
    auto doc = StreamJSON(stream, api);
    EXPECT_STREQ(doc["title_override"].GetString(), "Movie night");
    EXPECT_STREQ(doc["title"].GetString(), "Provider title");
  }

  // Simulate a poll and a profile edit while the card is already present.
  stream->SetTitle("Updated provider title");
  auto edited = std::make_shared<User>(*user);
  ASSERT_TRUE(edited->SetStreamTitleOverride("Another movie").Ok());
  ASSERT_TRUE(users->Save(edited).Ok());
  EXPECT_STREQ(StreamJSON(stream)["title_override"].GetString(), "Another movie");
  ASSERT_TRUE(stream->Save());
  auto reloaded_users = std::make_shared<Users>(db);
  Streams reloaded(db, reloaded_users);
  EXPECT_STREQ(StreamJSON(reloaded.GetByChannel(channel))["title_override"].GetString(),
               "Another movie");

  ASSERT_TRUE(edited->SetStreamTitleOverride("").Ok());
  ASSERT_TRUE(users->Save(edited).Ok());
  auto cleared = StreamJSON(stream);
  EXPECT_STREQ(cleared["title_override"].GetString(), "");
  EXPECT_STREQ(cleared["title"].GetString(), "Updated provider title");
}

TEST(StreamsTest, ProfileTitlesAreScopedToTheirOwnStream) {
  sqlite::database db(":memory:");
  auto users = std::make_shared<Users>(db);
  Streams streams(db, users);
  auto channel = Channel::Create("test", "angelthump", "movies");
  auto other_channel = Channel::Create("test", "angelthump", "other");
  auto user = users->Emplace(1, channel, "127.0.0.1");
  auto other = users->Emplace(2, other_channel, "127.0.0.1");
  ASSERT_NE(user, nullptr);
  ASSERT_NE(other, nullptr);
  ASSERT_TRUE(user->SetStreamTitleOverride("Movie night").Ok());
  ASSERT_TRUE(users->Save(user).Ok());
  ASSERT_TRUE(other->SetStreamTitleOverride("Other title").Ok());
  ASSERT_TRUE(users->Save(other).Ok());
  auto stream = streams.Emplace(channel);
  EXPECT_STREQ(StreamJSON(stream)["title_override"].GetString(), "Movie night");
  EXPECT_STREQ(StreamJSON(streams.Emplace(other_channel))["title_override"].GetString(),
               "Other title");
  EXPECT_STREQ(StreamJSON(streams.Emplace(Channel::Create("test", "angelthump")))
                   ["title_override"].GetString(), "");
  EXPECT_STREQ(StreamJSON(streams.Emplace(Channel::Create("test", "twitch", "movies")))
                   ["title_override"].GetString(), "");

  auto edited = std::make_shared<User>(*user);
  auto new_channel = Channel::Create("different", "angelthump", "movies");
  edited->SetChannel(new_channel);
  ASSERT_TRUE(users->Save(edited).Ok());
  EXPECT_STREQ(StreamJSON(stream)["title_override"].GetString(), "");
  EXPECT_STREQ(StreamJSON(streams.Emplace(new_channel))["title_override"].GetString(),
               "Movie night");
}

} // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
