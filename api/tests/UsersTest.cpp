#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sqlite_modern_cpp.h>

#include "../src/Config.h"
#include "../src/Users.h"

namespace rustla2 {

TEST(UsersTest, TestSetName) {
  Config::Get().Init("users_test.env");

  sqlite::database db(":memory:");

  auto channel = Channel::Create("twitch", "test");
  auto chat_channel = Channel::Create("strims", "");

  auto names = std::vector<std::string>{"InfiniteJester", "beepybeepy",
                                        "SpiderTechnitian"};
  for (const auto &name : names) {
    User user(db, 1, channel, chat_channel, "10.0.0.1");
    auto status = user.SetName(name);
    LOG(INFO) << status.GetErrorMessage();
    EXPECT_TRUE(status.Ok());
  }
}

}  // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
