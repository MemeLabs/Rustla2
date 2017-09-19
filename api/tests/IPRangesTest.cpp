#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sqlite_modern_cpp.h>
#include <chrono>

#include "../src/IPRanges.h"

namespace rustla2 {

TEST(IPRangesTest, TestV4) {
  sqlite::database db(":memory:");
  IPRanges test_ranges(db, "ip_ranges");

  test_ranges.Insert("127.0.0.25", "127.0.0.50");
  test_ranges.Insert("127.0.0.25", "127.0.0.100");
  test_ranges.Insert("127.0.2.1", "127.0.5.1");

  EXPECT_TRUE(test_ranges.Contains("127.0.0.25"));
  EXPECT_TRUE(test_ranges.Contains("127.0.0.30"));
  EXPECT_TRUE(test_ranges.Contains("::ffff:127.0.0.30"));
  EXPECT_TRUE(test_ranges.Contains("127.0.3.1"));

  EXPECT_FALSE(test_ranges.Contains("127.0.0.10"));
  EXPECT_FALSE(test_ranges.Contains("127.1.0.1"));
  EXPECT_FALSE(test_ranges.Contains("::ffff:127.1.0.1"));

  EXPECT_FALSE(test_ranges.Contains("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
}

TEST(IPRangesTest, TestV6) {
  sqlite::database db(":memory:");
  IPRanges test_ranges(db, "ip_ranges");

  test_ranges.Insert("2001:0000:0000:0000:0000:0000:0000:0000",
                     "2001:0000:0000:0000:ffff:0000:0000:0000");

  test_ranges.Insert("2001:0000:0000:0000:0000:0000:0000:0000",
                     "2001:0000:0000:ffff:0000:0000:0000:0000");

  EXPECT_TRUE(test_ranges.Contains("2001:0000:0000:0000:0000:0001:0000:0000"));

  EXPECT_FALSE(test_ranges.Contains("2001:0000:0001:0000:0000:0000:0000:0000"));

  EXPECT_FALSE(test_ranges.Contains("127.0.0.10"));
}

TEST(IPRangesTest, TestInvalidValues) {
  sqlite::database db(":memory:");
  IPRanges test_ranges(db, "ip_ranges");

  EXPECT_FALSE(test_ranges.Contains("some invalid value"));
  EXPECT_FALSE(test_ranges.Contains(""));
}

} // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
