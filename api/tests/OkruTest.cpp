#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include "../src/OkruClient.h"

namespace rustla2 {

TEST(OkruTest, OnlineBroadcast) {
  const std::string response = R"json({"movie": {
    "status": "ONLINE", "isLive": true, "title": "Test broadcast",
    "poster": "https://iv.okcdn.ru/i?r=poster&fn=external_8"
  }})json";

  okru::ChannelResult channel;
  ASSERT_TRUE(channel.SetData(response.c_str(), response.size()).Ok());
  EXPECT_TRUE(channel.GetLive());
  EXPECT_EQ(channel.GetTitle(), "Test broadcast");
  EXPECT_EQ(channel.GetThumbnail(),
            "https://iv.okcdn.ru/i?r=poster&fn=external_8");
}

TEST(OkruTest, PausedBroadcastWithPlaybackURLsIsOffline) {
  const std::string response = R"json({
    "movie": {"status": "OFFLINE", "isLive": true},
    "hlsMasterPlaylistUrl": "https://example.com/live.m3u8",
    "liveDashManifestUrl": "https://example.com/live.mpd"
  })json";

  okru::ChannelResult channel;
  ASSERT_TRUE(channel.SetData(response.c_str(), response.size()).Ok());
  EXPECT_FALSE(channel.GetLive());
  EXPECT_TRUE(channel.GetTitle().empty());
  EXPECT_TRUE(channel.GetThumbnail().empty());
}

TEST(OkruTest, RecordingsAndUnknownStatusesAreNotLive) {
  const std::string responses[] = {
      R"({"movie":{"status":"OK","isLive":true}})",
      R"({"movie":{"status":"ONLINE","isLive":false}})",
      R"({"movie":{"status":"UNKNOWN","isLive":true}})"};
  for (const auto& response : responses) {
    okru::ChannelResult channel;
    ASSERT_TRUE(channel.SetData(response.c_str(), response.size()).Ok());
    EXPECT_FALSE(channel.GetLive()) << response;
  }
}

TEST(OkruTest, NullTitleAndPosterAreOptional) {
  const std::string response = R"json({"movie": {
    "status": "ONLINE", "isLive": true, "title": null, "poster": null
  }})json";

  okru::ChannelResult channel;
  ASSERT_TRUE(channel.SetData(response.c_str(), response.size()).Ok());
  EXPECT_TRUE(channel.GetLive());
  EXPECT_TRUE(channel.GetTitle().empty());
  EXPECT_TRUE(channel.GetThumbnail().empty());
}

TEST(OkruTest, RejectsMalformedAndErrorResponses) {
  const std::string responses[] = {
      "<html>Unavailable</html>",
      R"({"error":"Video not found"})",
      R"({"movie":null})",
      R"({"movie":{"isLive":true}})",
      R"({"movie":{"status":"ONLINE"}})",
      R"({"movie":{"status":true,"isLive":true}})",
      R"({"movie":{"status":"ONLINE","isLive":"true"}})",
      R"({"movie":{"status":"ONLINE","isLive":true,"poster":42}})"};
  for (const auto& response : responses) {
    okru::ChannelResult channel;
    EXPECT_FALSE(channel.SetData(response.c_str(), response.size()).Ok())
        << response;
  }
}

TEST(OkruTest, RejectsInvalidIDsBeforeRequestingMetadata) {
  okru::Client client;
  okru::ChannelResult channel;
  EXPECT_FALSE(client.GetChannelByID("", &channel).Ok());
  EXPECT_FALSE(client.GetChannelByID("not-a-broadcast", &channel).Ok());
  EXPECT_FALSE(client.GetChannelByID("123&other=456", &channel).Ok());
}

}  // namespace rustla2

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
