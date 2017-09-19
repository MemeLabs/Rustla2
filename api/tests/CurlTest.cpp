#include <gtest/gtest.h>
#include <cstdlib>

#include "../src/Curl.h"

namespace rustla2 {

TEST(CurlTest, Test) {
  CurlRequest req("https://www.google.com/");
  req.Submit();

  EXPECT_EQ(req.GetResponseCode(), 200);
}

}  // namespace rustla2

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
