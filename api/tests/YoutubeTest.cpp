#include <gtest/gtest.h>

#include <cstdlib>

#include "../src/YoutubeClient.h"

namespace rustla2 {

TEST(YoutubeTest, TestNSFW) {
  std::string resp = R"json(
    {
     "kind": "youtube#videoListResponse",
     "etag": "\"Dn5xIderbhAnUk5TAW0qkFFir0M/N9Z7VDH9uxY4rgRIeuS6KORQFAY\"",
     "pageInfo": {
      "totalResults": 1,
      "resultsPerPage": 1
     },
     "items": [
      {
       "kind": "youtube#video",
       "etag": "\"Dn5xIderbhAnUk5TAW0qkFFir0M/gEfkNjMM6aJzyJ_4LyCoV9FyV2k\"",
       "id": "8pEpH1JWyiQ",
       "snippet": {
        "publishedAt": "2020-03-19T14:16:48.000Z",
        "channelId": "UCwg-SI_bIi6zFRaeP8_ZGkw",
        "title": "Testing",
        "thumbnails": {
         "default": {
          "url": "https://i.ytimg.com/vi/8pEp1JWyiQ/default.jpg",
          "width": 120,
          "height": 90
         },
         "medium": {
          "url": "https://i.ytimg.com/vi/8pep1jwyiq/mqdefault.jpg",
          "width": 320,
          "height": 180
         },
         "high": {
          "url": "https://i.ytimg.com/vi/8pEp1JWyiQ/hqdefault.jpg",
          "width": 480,
          "height": 360
         },
         "standard": {
          "url": "https://i.ytimg.com/vi/8pEp1JWyiQ/sddefault.jpg",
          "width": 640,
          "height": 480
         }
        },
        "channelTitle": "HELLO TODAY",
        "categoryId": "27",
        "liveBroadcastContent": "none"
       },
       "contentDetails": {
        "contentRating": {
         "ytRating": "ytAgeRestricted"
        },
        "projection": "rectangular"
       },
       "statistics": {
        "viewCount": "444788"
       }
      }
     ]
    }
  )json";
  youtube::VideosResult test_results;
  auto status = test_results.SetData(resp.c_str(), resp.size());
  if (status.Ok()) {
    auto first = test_results.GetVideo(0);
    EXPECT_EQ(test_results.GetTotalResults(), 1);
    EXPECT_EQ(first.GetMediumThumbnail(),
              "https://i.ytimg.com/vi/8pep1jwyiq/mqdefault.jpg");
    EXPECT_EQ(first.GetTitle(), "Testing");
    EXPECT_TRUE(first.IsNSFW());
  } else {
    std::cout << status.GetErrorMessage();
    FAIL();
  }
}

TEST(YoutubeTest, TestNotNSFW) {
  std::string resp = R"json(
    {
     "kind": "youtube#videoListResponse",
     "etag": "\"Dn5xIderbhAnUk5TAW0qkFFir0M/N9Z7VDH9uxY4rgRIeuS6KORQFAY\"",
     "pageInfo": {
      "totalResults": 1,
      "resultsPerPage": 1
     },
     "items": [
      {
       "kind": "youtube#video",
       "etag": "\"Dn5xIderbhAnUk5TAW0qkFFir0M/gEfkNjMM6aJzyJ_4LyCoV9FyV2k\"",
       "id": "8pEpH1JWyiQ",
       "snippet": {
        "publishedAt": "2020-03-19T14:16:48.000Z",
        "channelId": "UCwg-SI_bIi6zFRaeP8_ZGkw",
        "title": "Testing",
        "thumbnails": {
         "default": {
          "url": "https://i.ytimg.com/vi/8pEp1JWyiQ/default.jpg",
          "width": 120,
          "height": 90
         },
         "medium": {
          "url": "https://i.ytimg.com/vi/8pep1jwyiq/mqdefault.jpg",
          "width": 320,
          "height": 180
         },
         "high": {
          "url": "https://i.ytimg.com/vi/8pEp1JWyiQ/hqdefault.jpg",
          "width": 480,
          "height": 360
         },
         "standard": {
          "url": "https://i.ytimg.com/vi/8pEp1JWyiQ/sddefault.jpg",
          "width": 640,
          "height": 480
         }
        },
        "channelTitle": "HELLO TODAY",
        "categoryId": "27",
        "liveBroadcastContent": "none"
       },
       "contentDetails": {},
       "statistics": {
        "viewCount": "444788"
       }
      }
     ]
    }
  )json";
  youtube::VideosResult test_results;
  auto status = test_results.SetData(resp.c_str(), resp.size());
  if (status.Ok()) {
    auto first = test_results.GetVideo(0);
    EXPECT_EQ(test_results.GetTotalResults(), 1);
    EXPECT_EQ(first.GetMediumThumbnail(),
              "https://i.ytimg.com/vi/8pep1jwyiq/mqdefault.jpg");
    EXPECT_EQ(first.GetTitle(), "Testing");
    EXPECT_FALSE(first.IsNSFW());
  } else {
    std::cout << status.GetErrorMessage();
    FAIL();
  }
}

} // namespace rustla2

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
