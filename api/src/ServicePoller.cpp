#include "ServicePoller.h"

#include "AngelThumpClient.h"
#include "Config.h"

namespace rustla2 {

ServicePoller::ServicePoller(std::shared_ptr<DB> db) : db_(db) {
  twitch::ClientConfig twitch_config{
      .client_id = Config::Get().GetTwitchClientID(),
      .client_secret = Config::Get().GetTwitchClientSecret(),
      .redirect_uri = Config::Get().GetTwitchRedirectURL()};
  twitch_.reset(new twitch::Client(twitch_config));

  youtube::ClientConfig youtube_config{
      .public_api_key = Config::Get().GetGooglePublicAPIKey()};
  youtube_.reset(new youtube::Client(youtube_config));
}

void ServicePoller::Run() {
  auto streams = db_->GetStreams()->GetAllWithRustlers();
  for (const auto& stream : streams) {
    ChannelState state;
    APIStatus status;

    auto service = stream->GetService();
    auto channel = stream->GetChannel();
    if (service == kTwitchService) {
      status = CheckTwitchStream(channel, &state);
    } else if (service == kTwitchVODService) {
      status = CheckTwitchVOD(channel, &state);
    } else if (service == kAngelThumpService) {
      status = CheckAngelThump(channel, &state);
    } else if (service == kYouTubeService) {
      status = CheckYouTube(channel, &state);
    }

    if (status.Ok()) {
      stream->SetLive(state.live);
      stream->SetThumbnail(state.thumbnail);
      stream->SetViewerCount(state.viewers);
      stream->Save();
    }
  }
}

const APIStatus ServicePoller::CheckAngelThump(const std::string& name,
                                               ChannelState* state) {
  angelthump::Client client;
  angelthump::ChannelResult channel;
  auto status = client.GetChannelByName(name, &channel);

  if (!status.Ok()) {
    return status;
  }

  state->live = channel.GetLive();
  state->thumbnail = channel.GetThumbnail();
  state->viewers = channel.GetViewers();
}

const APIStatus ServicePoller::CheckTwitchStream(const std::string& name,
                                                 ChannelState* state) {
  twitch::UsersResult users;
  auto user_status = twitch_->GetUsersByName(name, &users);
  if (!user_status.Ok()) {
    return user_status;
  }

  if (users.IsEmpty()) {
    return APIStatus(StatusCode::ERROR, "Invalid login: " + name,
                     "Twitch API did not return a user matching this login");
  }

  auto user_id = users.GetUser(0).GetID();
  twitch::StreamsResult stream;
  auto stream_status = twitch_->GetStreamByID(user_id, &stream);
  if (!stream_status.Ok()) {
    return stream_status;
  }

  if (!stream.IsEmpty()) {
    state->live = true;
    state->thumbnail = stream.GetLargePreview();
    state->viewers = stream.GetViewers();
  } else {
    twitch::ChannelsResult channel;
    auto channel_status = twitch_->GetChannelByID(user_id, &channel);
    if (!channel_status.Ok()) {
      return channel_status;
    }

    state->live = false;
    state->thumbnail = channel.GetVideoBanner();
    state->viewers = 0;
  }

  return APIStatus::OK;
}

const APIStatus ServicePoller::CheckTwitchVOD(const std::string& name,
                                              ChannelState* state) {
  twitch::VideosResult videos;
  auto status = twitch_->GetVideosByID("v" + name, &videos);

  if (status.Ok()) {
    state->live = true;
    state->thumbnail = videos.GetLargePreview();
    state->viewers = videos.GetViews();
  }

  return status;
}

const APIStatus ServicePoller::CheckYouTube(const std::string& name,
                                            ChannelState* state) {
  youtube::VideosResult videos;
  auto status = youtube_->GetVideosByID(name, &videos);

  if (status.Ok() && !videos.IsEmpty()) {
    auto video = videos.GetVideo(0);
    state->live = true;
    state->thumbnail = video.GetMediumThumbnail();
    state->viewers = video.GetViewers();
  }

  return status;
}

}  // namespace rustla2
