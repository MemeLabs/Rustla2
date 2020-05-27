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
#include "Observer.h"
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

  std::string user_id{""};
  std::string name{""};
  bool online{false};
  bool enable_public_state{false};
  std::uint64_t stream_id{0};
  std::shared_ptr<Channel> channel;
};

using ViewerStateObserver = Observer<std::string>;

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

  void MarkUserChanged(const std::string &user_id);

  bool GetNextUserState(std::shared_ptr<ViewerStateObserver> observer,
                        UserState *state);

 private:
  std::shared_ptr<Users> users_;
  std::shared_ptr<Streams> streams_;
  boost::mutex lock_;
  ViewerMap data_;
  Observable<std::string> user_change_observers_;
};

}  // namespace rustla2
