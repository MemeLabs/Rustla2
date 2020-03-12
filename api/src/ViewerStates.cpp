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

void ViewerStates::IncrViewerStream(const std::string &user_id,
                                    const std::uint64_t stream_id) {
  if (user_id.empty()) {
    return;
  }

  boost::lock_guard<boost::mutex> lock{lock_};

  data_[{user_id, stream_id}]++;

  user_change_observers_.Mark(user_id);
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

  user_change_observers_.Mark(user_id);
}

std::shared_ptr<ViewerStateObserver> ViewerStates::CreateObserver() {
  boost::lock_guard<boost::mutex> lock{lock_};

  std::set<std::string> ids;
  std::transform(data_.begin(), data_.end(), std::inserter(ids, ids.begin()),
                 [&](auto i) { return i.first.user_id; });

  return user_change_observers_.CreateObserver(ids);
}

void ViewerStates::StopObserving(
    std::shared_ptr<ViewerStateObserver> observer) {
  user_change_observers_.StopObserving(observer);
}

void ViewerStates::MarkUserChanged(const std::string &user_id) {
  user_change_observers_.Mark(user_id);
}

bool ViewerStates::GetNextUserState(
    std::shared_ptr<ViewerStateObserver> observer, UserState *state) {
  UserState new_state;

  if (!observer->Next(&new_state.user_id)) {
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

}  // namespace rustla2
