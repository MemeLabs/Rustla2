#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <cstdlib>

#include "../src/SmashcastClient.h"

namespace rustla2 {

TEST(smashcastTest, TestNSFW) {
  std::string resp = R"json(
    {
    "livestream": [
      {
        "media_is_live": "1",
        "media_status": "Artificial Intelligence Tournament",
        "media_views": "128",
        "media_thumbnail": "/static/img/media/live/sscaitournament_mid_000.jpg"
      }
    ]
  }
  )json";

  smashcast::ChannelResult chnl;
  auto status = chnl.SetData(resp.c_str(), resp.size());
  if (status.Ok()) {
    EXPECT_EQ(chnl.GetViewers(), 128);
    EXPECT_STREQ(chnl.GetThumbnail().c_str(),
                 "https://edge.sf.hitbox.tv/static/img/media/live/sscaitournament_mid_000.jpg");
    EXPECT_STREQ(chnl.GetTitle().c_str(), "Artificial Intelligence Tournament");
    EXPECT_TRUE(chnl.GetLive());
  } else {
    LOG(INFO) << status.GetErrorMessage();
    LOG(INFO) << status.GetErrorDetails();
    FAIL();
  }
}

} // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
