#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <cstdlib>

#include "../src/AngelThumpClient.h"

namespace rustla2 {

TEST(AngelthumpTest, TestNSFW) {
  std::string resp = R"json(
    [
      {
        "type": "live",
        "thumbnail_url": "https://thumbnail.angelthump.com/thumbnails/psrngafk.jpeg",
        "viewer_count": "38",
        "user": {
          "offline_banner_url": "https://images-angelthump.nyc3.cdn.digitaloceanspaces.com/offline-banners/31dc54b09264952f60fcdd6b7b743920be725a74a1026a858b81b259cfc79fc6.png",
          "title": "Arrival (2016)",
          "nsfw": true
        }
      }
    ]
  )json";

  angelthump::ChannelResult chnl;
  auto status = chnl.SetData(resp.c_str(), resp.size());
  if (status.Ok()) {
    EXPECT_EQ(chnl.GetViewers(), 38);
    EXPECT_STREQ(chnl.GetThumbnail().c_str(),
                 "https://thumbnail.angelthump.com/thumbnails/psrngafk.jpeg");
    EXPECT_STREQ(chnl.GetTitle().c_str(), "Arrival (2016)");
    EXPECT_TRUE(chnl.IsNSFW());
  } else {
    LOG(INFO) << status.GetErrorMessage();
    LOG(INFO) << status.GetErrorDetails();
    FAIL();
  }
}

TEST(AngelthumpTest, TestNotNSFW) {
  std::string resp = R"json(
    [
      {
        "type": "live",
        "thumbnail_url": "https://thumbnail.angelthump.com/thumbnails/psrngafk.jpeg",
        "viewer_count": "34",
        "user": {
          "offline_banner_url": "https://images-angelthump.nyc3.cdn.digitaloceanspaces.com/offline-banners/31dc54b09264952f60fcdd6b7b743920be725a74a1026a858b81b259cfc79fc6.png",
          "title": "Arrival (2016)",
          "nsfw": false
        }
      }
    ]
  )json";

  angelthump::ChannelResult chnl;
  auto status = chnl.SetData(resp.c_str(), resp.size());
  if (status.Ok()) {
    EXPECT_EQ(chnl.GetViewers(), 34);
    EXPECT_STREQ(chnl.GetThumbnail().c_str(),
                 "https://thumbnail.angelthump.com/thumbnails/psrngafk.jpeg");
    EXPECT_STREQ(chnl.GetTitle().c_str(), "Arrival (2016)");
    EXPECT_FALSE(chnl.IsNSFW());
  } else {
    LOG(INFO) << status.GetErrorMessage();
    LOG(INFO) << status.GetErrorDetails();
    FAIL();
  }
}

}  // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
