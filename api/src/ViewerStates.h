#pragma once

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <boost/thread/mutex.hpp>
#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <tuple>

#include "Channel.h"
#include "Streams.h"
#include "Users.h"

namespace rustla2 {

struct ViewerKey {
  std::string user_id;
  std::uint64_t stream_id;
};

struct ViewerKeyLess : public std::binary_function<ViewerKey, ViewerKey, bool> {
  bool operator()(const ViewerKey &a, const ViewerKey &b) const {
    return a.user_id == b.user_id ? a.stream_id < b.stream_id
                                  : a.user_id < b.user_id;
  }
};

using ViewerMap = std::map<ViewerKey, uint32_t, ViewerKeyLess>;

struct UserState {
  void WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer);

  std::string user_id = "";
  std::string alias = "";
  bool online = false;
  bool enable_public_state = false;
  std::uint64_t stream_id = 0;
  std::shared_ptr<Channel> channel = nullptr;
};

class ViewerStateObserver {
 public:
  ViewerStateObserver(std::uint64_t id) : id_(id){};

 private:
  std::uint64_t GetID() { return id_; }

  void MarkUserChanged(const std::string &user_id);

  bool GetNextUserID(std::string *user_id);

  boost::mutex lock_;
  std::uint64_t id_;
  std::set<std::string> changed_user_ids_;

  friend class ViewerStates;
};

class ViewerStates {
 public:
  explicit ViewerStates(std::shared_ptr<Users> users,
                        std::shared_ptr<Streams> streams)
      : users_(users), streams_(streams){};

  void IncrViewerStream(const std::string &user_id,
                        const std::uint64_t stream_id);

  void DecrViewerStream(const std::string &user_id,
                        const std::uint64_t stream_id);

  std::shared_ptr<ViewerStateObserver> CreateObserver();

  void StopObserving(std::shared_ptr<ViewerStateObserver> observer);

  bool GetNextUserState(std::shared_ptr<ViewerStateObserver> observer,
                        UserState *state);

 private:
  void MarkUserChanged(const std::string &user_id);

  std::shared_ptr<Users> users_;
  std::shared_ptr<Streams> streams_;
  boost::mutex lock_;
  ViewerMap data_;
  std::uint64_t observer_id_;
  std::map<std::uint64_t, std::shared_ptr<ViewerStateObserver>> observers_;
};

}  // namespace rustla2
