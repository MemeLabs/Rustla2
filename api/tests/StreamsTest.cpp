#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sqlite_modern_cpp.h>

#include "../src/Observer.h"
#include "../src/Streams.h"
#include "rapidjson/document.h"

namespace rustla2 {

TEST(StreamsTest, TestWriteAPIJSON) {
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
}  // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}