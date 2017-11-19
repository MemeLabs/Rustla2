#pragma once

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <sqlite_modern_cpp.h>
#include <boost/thread/shared_mutex.hpp>
#include <memory>
#include <unordered_map>

#include "Channel.h"

namespace rustla2 {

class User {
 public:
  User(sqlite::database db, const std::string &id, const Channel &channel,
       const std::string &last_ip, const time_t last_seen, const bool left_chat,
       const bool is_admin)
      : db_(db),
        id_(id),
        channel_(std::shared_ptr<Channel>(channel)),
        last_ip_(last_ip),
        last_seen_(last_seen),
        left_chat_(left_chat),
        is_admin_(is_admin) {}

  User(sqlite::database db, const std::string &id, const Channel &channel,
       const std::string &last_ip)
      : db_(db),
        id_(id),
        channel_(std::shared_ptr<Channel>(channel)),
        last_ip_(last_ip),
        last_seen_(time(nullptr)),
        left_chat_(false),
        is_admin_(false) {}

  inline std::string GetID() {
    boost::shared_lock<boost::shared_mutex> read_lock(lock_);
    return id_;
  }

  inline std::shared_ptr<Channel> GetChannel() {
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

  void WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

  bool SetChannel(const Channel &channel);

  bool SetLeftChat(bool left_chat);

  bool SetLastIP(const std::string &last_ip);

  bool SetLastSeen(const time_t last_seen);

  bool Save();

  bool SaveNew();

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::string id_;
  std::shared_ptr<Channel> channel_;
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

  std::shared_ptr<User> Emplace(const std::string &name, const Channel &channel,
                                const std::string &ip);

  void WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

 private:
  sqlite::database db_;
  boost::shared_mutex lock_;
  std::unordered_map<std::string, std::shared_ptr<User>> data_;
};

}  // namespace rustla2
