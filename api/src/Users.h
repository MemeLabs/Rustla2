#pragma once

#include <sqlite_modern_cpp.h>
#include <boost/thread/shared_mutex.hpp>
#include <unordered_map>

namespace rustla2 {

class User {
 public:
  User(sqlite::database db, const std::string &id, const std::string &service,
       const std::string &channel, const std::string &last_ip,
       const time_t last_seen, const bool left_chat, const bool is_admin)
      : db_(db),
        id_(id),
        service_(service),
        channel_(channel),
        last_ip_(last_ip),
        last_seen_(last_seen),
        left_chat_(left_chat),
        is_admin_(is_admin) {}

  User(sqlite::database db, const std::string &id, const std::string &service,
       const std::string &channel, const std::string &last_ip)
      : db_(db),
        id_(id),
        service_(service),
        channel_(channel),
        last_ip_(last_ip),
        last_seen_(time(nullptr)),
        left_chat_(false),
        is_admin_(false) {}

  inline std::string GetID() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return id_;
  }

  inline std::string GetService() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return service_;
  }

  inline std::string GetChannel() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return channel_;
  }

  inline std::string GetLastIP() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return last_ip_;
  }

  inline time_t GetLastSeen() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return last_seen_;
  }

  inline bool GetLeftChat() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return left_chat_;
  }

  inline bool GetIsAdmin() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return is_admin_;
  }

  std::string GetStreamJSON();

  std::string GetProfileJSON();

  bool SetChannelAndService(const std::string &channel,
                            const std::string &service);

  bool SetLeftChat(bool left_chat);

  bool SetLastIP(const std::string &last_ip);

  bool SetLastSeen(const time_t last_seen);

  bool Save();

  bool SaveNew();

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::string id_;
  std::string service_;
  std::string channel_;
  std::string last_ip_;
  time_t last_seen_;
  bool left_chat_;
  bool is_admin_;
};

class Users {
 public:
  Users(sqlite::database db);

  void InitTable();

  std::shared_ptr<User> GetByName(const std::string &name);

  std::shared_ptr<User> Emplace(const std::string &name,
                                const std::string &service,
                                const std::string &channel,
                                const std::string &ip);

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::unordered_map<std::string, std::shared_ptr<User>> data_;
};

}  // rustla2
