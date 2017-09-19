#pragma once

#include <sqlite_modern_cpp.h>
#include <memory>

#include "BannedStreams.h"
#include "Config.h"
#include "IPRanges.h"
#include "Streams.h"
#include "Users.h"

namespace rustla2 {

class DB {
 public:
  DB()
      : db_(Config::Get().GetDBPath()),
        users_(std::make_shared<Users>(db_)),
        banned_streams_(std::make_shared<BannedStreams>(db_)),
        banned_ips_(std::make_shared<IPRanges>(db_, "banned_ip_ranges")),
        streams_(std::make_shared<Streams>(db_)) {}

  inline std::shared_ptr<Users> GetUsers() { return users_; }

  inline std::shared_ptr<BannedStreams> GetBannedStreams() {
    return banned_streams_;
  }

  inline std::shared_ptr<IPRanges> GetBannedIPs() { return banned_ips_; }

  inline std::shared_ptr<Streams> GetStreams() { return streams_; }

 private:
  sqlite::database db_;
  std::shared_ptr<Users> users_;
  std::shared_ptr<BannedStreams> banned_streams_;
  std::shared_ptr<IPRanges> banned_ips_;
  std::shared_ptr<Streams> streams_;
};

}  // rustla2
