#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <cstdlib>

#include "../src/MixerClient.h"

namespace rustla2 {

TEST(MixerTest, TestNSFW) {
  std::string resp = R"json(
    {
      "online": true,
      "viewersCurrent": 377,
      "name": "test",
      "audience": "18+",
      "thumbnail": {
        "url": "https://uploads.mixer.com/thumbnails/s736odg5-7968657.jpg"
      }
    }
  )json";

  mixer::ChannelResult chnl;
  auto status = chnl.SetData(resp.c_str(), resp.size());
  if (status.Ok()) {
    EXPECT_EQ(chnl.GetViewers(), 377);
    EXPECT_STREQ(chnl.GetThumbnail().c_str(),
                 "https://uploads.mixer.com/thumbnails/s736odg5-7968657.jpg");
    EXPECT_STREQ(chnl.GetName().c_str(), "test");
    EXPECT_TRUE(chnl.IsNSFW());
  } else {
    LOG(INFO) << status.GetErrorMessage();
    LOG(INFO) << status.GetErrorDetails();
    FAIL();
  }
}

TEST(MixerTest, TestNotNSFW) {
  std::string resp = R"json(
    {
      "online": true,
      "viewersCurrent": 377,
      "name": "test",
      "audience": "teen",
      "thumbnail": {
        "url": "https://uploads.mixer.com/thumbnails/s736odg5-7968657.jpg"
      }
    }
  )json";

  mixer::ChannelResult chnl;
  auto status = chnl.SetData(resp.c_str(), resp.size());
  if (status.Ok()) {
    EXPECT_EQ(chnl.GetViewers(), 377);
    EXPECT_STREQ(chnl.GetThumbnail().c_str(),
                 "https://uploads.mixer.com/thumbnails/s736odg5-7968657.jpg");
    EXPECT_STREQ(chnl.GetName().c_str(), "test");
    EXPECT_FALSE(chnl.IsNSFW());
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
