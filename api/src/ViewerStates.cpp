#include "ViewerStates.h"

namespace rustla2 {

void UserState::WriteJSON(rapidjson::Writer<rapidjson::StringBuffer> *writer) {
  writer->StartObject();
  writer->Key("user_id");
  writer->String(user_id);
  writer->Key("name");
  writer->String(name);
  writer->Key("online");
  writer->Bool(online);
  writer->Key("enable_public_state");
  writer->Bool(enable_public_state);
  writer->Key("stream_id");
  writer->Uint64(stream_id);

  if (channel != nullptr) {
    writer->Key("channel");
    channel->WriteJSON(writer);
  }

  writer->EndObject();
}

void ViewerStateObserver::MarkUserChanged(const std::string &user_id) {
  boost::lock_guard<boost::mutex> lock{lock_};
  changed_user_ids_.insert(user_id);
}

bool ViewerStateObserver::GetNextUserID(std::string *user_id) {
  boost::lock_guard<boost::mutex> lock{lock_};

  auto next_user_id = changed_user_ids_.begin();
  if (next_user_id == changed_user_ids_.end()) {
    return false;
  }

  *user_id = *next_user_id;
  changed_user_ids_.erase(*user_id);

  return true;
}

void ViewerStates::IncrViewerStream(const std::string &user_id,
                                    const std::uint64_t stream_id) {
  if (user_id.empty()) {
    return;
  }

  boost::lock_guard<boost::mutex> lock{lock_};

  data_[{user_id, stream_id}]++;

  MarkUserChanged(user_id);
}

void ViewerStates::DecrViewerStream(const std::string &user_id,
                                    const std::uint64_t stream_id) {
  if (user_id.empty()) {
    return;
  }

  boost::lock_guard<boost::mutex> lock{lock_};

  auto session_count = --data_[{user_id, stream_id}];
  if (session_count <= 0) {
    data_.erase({user_id, stream_id});
  }

  MarkUserChanged(user_id);
}

void ViewerStates::MarkUserChanged(const std::string &user_id) {
  for (auto i : observers_) {
    i.second->MarkUserChanged(user_id);
  }
}

std::shared_ptr<ViewerStateObserver> ViewerStates::CreateObserver() {
  boost::lock_guard<boost::mutex> lock{lock_};

  auto id = observer_id_++;
  auto observer = std::make_shared<ViewerStateObserver>(id);
  observers_[id] = observer;

  for (auto i : data_) {
    observer->changed_user_ids_.insert(i.first.user_id);
  }

  return observer;
}

bool ViewerStates::GetNextUserState(
    std::shared_ptr<ViewerStateObserver> observer, UserState *state) {
  UserState new_state;

  if (!observer->GetNextUserID(&new_state.user_id)) {
    return false;
  }

  boost::lock_guard<boost::mutex> lock{lock_};

  auto user = users_->GetByID(new_state.user_id);
  if (user != nullptr) {
    new_state.name = user->GetName();
    new_state.enable_public_state = user->GetEnablePublicState();
  }

  auto user_id_state = data_.upper_bound({new_state.user_id, UINT64_MAX});
  if (user_id_state != data_.begin()) {
    user_id_state--;
    if (user_id_state->first.user_id == new_state.user_id) {
      new_state.stream_id = user_id_state->first.stream_id;
      new_state.online = true;

      auto stream = streams_->GetByID(new_state.stream_id);
      if (stream != nullptr) {
        new_state.channel = stream->GetChannel();
      }
    }
  }

  *state = new_state;
  return true;
}

void ViewerStates::StopObserving(
    std::shared_ptr<ViewerStateObserver> observer) {
  boost::lock_guard<boost::mutex> lock{lock_};

  observers_.erase(observer->GetID());
}

}  // namespace rustla2
