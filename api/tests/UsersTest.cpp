#include <gflags/gflags.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <sqlite_modern_cpp.h>
#include <rapidjson/document.h>

#include "../src/Config.h"
#include "../src/Users.h"

namespace rustla2 {

TEST(UsersTest, TestSetName) {
  Config::Get().Init("users_test.env");

  sqlite::database db(":memory:");

  auto channel = Channel::Create("twitch", "test");

  auto names = std::vector<std::string>{"InfiniteJester", "beepybeepy",
                                        "SpiderTechnitian"};
  for (const auto &name : names) {
    User user(db, 1, channel, "10.0.0.1");
    auto status = user.SetName(name);
    LOG(INFO) << status.GetErrorMessage();
    EXPECT_TRUE(status.Ok());
  }
}

TEST(UsersTest, StreamTitleValidation) {
  sqlite::database db(":memory:");
  User user(db, 1, Channel::Create("test", "angelthump"), "127.0.0.1");

  EXPECT_TRUE(user.SetStreamTitleOverride("  Movie night  ").Ok());
  EXPECT_EQ(user.GetStreamTitleOverride(), "Movie night");
  EXPECT_TRUE(user.SetStreamTitleOverride(u8"Кино 🎬").Ok());
  EXPECT_EQ(user.GetStreamTitleOverride(), u8"Кино 🎬");
  std::string unicode_title;
  for (int i = 0; i < 120; ++i) unicode_title += u8"🎬";
  EXPECT_TRUE(user.SetStreamTitleOverride(unicode_title).Ok());
  EXPECT_FALSE(user.SetStreamTitleOverride(unicode_title + "x").Ok());
  EXPECT_EQ(user.GetStreamTitleOverride(), unicode_title);
  EXPECT_FALSE(user.SetStreamTitleOverride("two\nlines").Ok());
  EXPECT_FALSE(user.SetStreamTitleOverride(std::string("a\0b", 3)).Ok());
  EXPECT_FALSE(user.SetStreamTitleOverride(std::string("\xff", 1)).Ok());
  EXPECT_TRUE(user.SetStreamTitleOverride("   ").Ok());
  EXPECT_EQ(user.GetStreamTitleOverride(), "");
}

TEST(UsersTest, StreamTitlePersistsAndClears) {
  sqlite::database db(":memory:");
  Users users(db);
  auto user = users.Emplace(1, Channel::Create("test", "angelthump", "movies"),
                            "127.0.0.1");
  ASSERT_NE(user, nullptr);
  EXPECT_EQ(user->GetStreamTitleOverride(), "");
  auto edited = std::make_shared<User>(*user);
  ASSERT_TRUE(edited->SetStreamTitleOverride("Movie night").Ok());
  ASSERT_TRUE(users.Save(edited).Ok());

  Users reloaded(db);
  auto saved = reloaded.GetByID(user->GetID());
  ASSERT_NE(saved, nullptr);
  EXPECT_EQ(saved->GetStreamTitleOverride(), "Movie night");
  rapidjson::Document profile;
  profile.Parse(saved->GetProfileJSON().c_str());
  EXPECT_STREQ(profile["stream_title_override"].GetString(), "Movie night");
  EXPECT_EQ(User(*saved).GetStreamTitleOverride(), "Movie night");

  ASSERT_TRUE(saved->SetStreamTitleOverride("").Ok());
  ASSERT_TRUE(reloaded.Save(saved).Ok());
  Users cleared(db);
  EXPECT_EQ(cleared.GetByID(user->GetID())->GetStreamTitleOverride(), "");
}

TEST(UsersTest, ExistingDatabaseUpgradePreservesProfilesAndRunsOnce) {
  sqlite::database db(":memory:");
  // Schema from before stream title overrides were added.
  db << R"sql(
    CREATE TABLE users (
      id CHAR(36) NOT NULL, twitch_id UNSIGNED BIGINT NOT NULL,
      twitch_username VARCHAR(32) NOT NULL, name VARCHAR(32) NOT NULL,
      stream_path VARCHAR(255) NOT NULL, service VARCHAR(255) NOT NULL,
      channel VARCHAR(255) NOT NULL, last_ip VARCHAR(255) NOT NULL,
      last_seen DATETIME NOT NULL, left_chat TINYINT(1) DEFAULT 0,
      is_banned TINYINT(1) NOT NULL DEFAULT 0, ban_reason VARCHAR(255),
      created_at DATETIME NOT NULL, updated_at DATETIME NOT NULL,
      is_admin TINYINT(1) DEFAULT 0, show_hidden TINYINT(1) DEFAULT 0,
      show_dgg_chat TINYINT(1) DEFAULT 0,
      enable_public_state TINYINT(1) DEFAULT 1,
      UNIQUE (id), UNIQUE (twitch_id)
    )
  )sql";
  db << R"sql(
    INSERT INTO users (id, twitch_id, twitch_username, name, stream_path,
      service, channel, last_ip, last_seen, created_at, updated_at, is_admin)
    VALUES ('11111111-2222-4333-8444-555555555555', 1, 'existing', 'existing',
      'movies', 'angelthump', 'test', '127.0.0.1', datetime(), datetime(),
      datetime(), 1)
  )sql";

  Users upgraded(db);
  auto user = upgraded.GetByTwitchID(1);
  ASSERT_NE(user, nullptr);
  EXPECT_EQ(user->GetName(), "existing");
  EXPECT_EQ(user->GetChannel()->GetStreamPath(), "movies");
  EXPECT_TRUE(user->GetIsAdmin());
  EXPECT_EQ(user->GetStreamTitleOverride(), "");
  ASSERT_TRUE(user->SetStreamTitleOverride("Saved after upgrade").Ok());
  ASSERT_TRUE(upgraded.Save(user).Ok());

  Users restarted(db);
  EXPECT_EQ(restarted.GetByTwitchID(1)->GetStreamTitleOverride(),
            "Saved after upgrade");
  int count = 0;
  db << "SELECT COUNT(*) FROM users" >> count;
  EXPECT_EQ(count, 1);
}

}  // namespace rustla2

int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, false);
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
