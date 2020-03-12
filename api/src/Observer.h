#pragma once

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <map>
#include <memory>

namespace rustla2 {

template <class T>
class Observer {
 public:
  Observer(std::uint64_t id) : id_(id){};

  std::uint64_t GetID() { return id_; }

  void Mark(const T &key) {
    boost::lock_guard<boost::mutex> lock{lock_};
    changed_keys_.insert(key);
  }

  bool Next(T *key) {
    boost::lock_guard<boost::mutex> lock{lock_};

    auto next_key = changed_keys_.begin();
    if (next_key == changed_keys_.end()) {
      return false;
    }

    *key = *next_key;
    changed_keys_.erase(*key);

    return true;
  }

 private:
  boost::mutex lock_;
  std::uint64_t id_;
  std::set<T> changed_keys_;
};

template <class T>
class Observable {
 public:
  std::shared_ptr<Observer<T>> CreateObserver(std::set<T> keys = {}) {
    boost::lock_guard<boost::mutex> lock{lock_};

    auto id = observer_id_++;
    auto observer = std::make_shared<Observer<T>>(id);
    observers_[id] = observer;

    return observer;
  }

  void StopObserving(std::shared_ptr<Observer<T>> observer) {
    boost::lock_guard<boost::mutex> lock{lock_};
    observers_.erase(observer->GetID());
  }

  void Mark(const T &key) {
    boost::lock_guard<boost::mutex> lock{lock_};
    for (auto i : observers_) {
      i.second->Mark(key);
    }
  }

 private:
  boost::mutex lock_;
  std::uint64_t observer_id_{0};
  std::map<std::uint64_t, std::shared_ptr<Observer<T>>> observers_;
};

}  // namespace rustla2
