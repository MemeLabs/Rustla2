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
  for (const auto &stream : streams) {
    ChannelState state;
    Status status;

    auto channel = stream->GetChannel();
    if (channel->GetService() == kTwitchService) {
      status = CheckTwitchStream(channel->GetChannel(), &state);
    } else if (channel->GetService() == kTwitchVODService) {
      status = CheckTwitchVOD(channel->GetChannel(), &state);
    } else if (channel->GetService() == kAngelThumpService) {
      status = CheckAngelThump(channel->GetChannel(), &state);
    } else if (channel->GetService() == kYouTubeService) {
      status = CheckYouTube(channel->GetChannel(), &state);
    } else if (channel->GetService() == kM3u8Service) {
      status = CheckM3u8(channel->GetChannel(), &state);
    } else if (channel->GetService() == kMixerService) {
      status = CheckMixer(channel->GetChannel(), &state);
    } else if (channel->GetService() == kSmashcastService) {
      status = CheckSmashcast(channel->GetChannel(), &state);
    }

    if (status.Ok()) {
      stream->SetTitle(state.title);
      stream->SetLive(state.live);
      stream->SetThumbnail(state.thumbnail);
      stream->SetViewerCount(state.viewers);
      stream->SetServiceNSFW(state.nsfw);
      stream->Save();
    } else {
      LOG(INFO) << status.GetErrorMessage() << ", " << status.GetErrorDetails();

      if (stream->GetLive()) {
        stream->SetLive(false);
        stream->Save();
      }
    }
  }
}

const Status ServicePoller::CheckAngelThump(const std::string &name,
                                            ChannelState *state) {
  angelthump::Client client;
  angelthump::ChannelResult channel;
  auto status = client.GetChannelByName(name, &channel);

  if (!status.Ok()) {
    return status;
  }

  state->title = channel.GetTitle();
  state->live = channel.GetLive();
  state->thumbnail = channel.GetThumbnail();
  state->viewers = channel.GetViewers();
  return status;
}

const Status ServicePoller::CheckM3u8(const std::string &name,
                                      ChannelState *state) {
  CurlRequest req(name);
  req.Submit();

  if (!req.Ok()) {
    return Status(StatusCode::HTTP_ERROR, req.GetErrorMessage());
  }

  state->live = true;

  return Status::OK;
}

const Status ServicePoller::CheckTwitchStream(const std::string &name,
                                              ChannelState *state) {
  twitch::UsersResult users;
  auto user_status = twitch_->GetUsersByName(name, &users);
  if (!user_status.Ok()) {
    return user_status;
  }

  if (users.IsEmpty()) {
    return Status(StatusCode::ERROR, "Invalid login: " + name,
                  "Twitch API did not return a user matching this login");
  }

  auto user_id = users.GetUser(0).GetID();
  twitch::StreamsResult stream;
  auto stream_status = twitch_->GetStreamByID(user_id, &stream);
  if (!stream_status.Ok()) {
    return stream_status;
  }

  if (!stream.IsEmpty()) {
    state->title = stream.GetGame();
    state->live = true;
    state->thumbnail = stream.GetLargePreview();
    state->viewers = stream.GetViewers();
  } else {
    twitch::ChannelsResult channel;
    auto channel_status = twitch_->GetChannelByID(user_id, &channel);
    if (!channel_status.Ok()) {
      return channel_status;
    }

    state->title = "";
    state->live = false;
    state->thumbnail = channel.GetVideoBanner();
    state->viewers = 0;
  }

  return Status::OK;
}

const Status ServicePoller::CheckTwitchVOD(const std::string &name,
                                           ChannelState *state) {
  twitch::VideosResult videos;
  auto status = twitch_->GetVideosByID("v" + name, &videos);

  if (status.Ok()) {
    state->title = videos.GetTitle();
    state->live = true;
    state->thumbnail = videos.GetLargePreview();
    state->viewers = videos.GetViews();
  }

  return status;
}

const Status ServicePoller::CheckYouTube(const std::string &name,
                                         ChannelState *state) {
  youtube::VideosResult videos;
  auto status = youtube_->GetVideosByID(name, &videos);

  if (status.Ok() && !videos.IsEmpty()) {
    auto video = videos.GetVideo(0);
    state->title = video.GetTitle();
    state->live = true;
    state->thumbnail = video.GetMediumThumbnail();
    state->viewers = video.GetViewers();
    state->nsfw = video.IsNSFW();
  }

  return status;
}

const Status ServicePoller::CheckMixer(const std::string &name,
                                       ChannelState *state) {
  mixer::Client client;
  mixer::ChannelResult channel;
  auto status = client.GetChannelByName(name, &channel);

  if (!status.Ok()) {
    return status;
  }

  state->title = channel.GetName();
  state->live = channel.GetLive();
  state->thumbnail = channel.GetThumbnail();
  state->viewers = channel.GetViewers();
  return status;
}

const Status ServicePoller::CheckSmashcast(const std::string &name,
                                           ChannelState *state) {
  smashcast::Client client;
  smashcast::ChannelResult channel;
  auto status = client.GetChannelByName(name, &channel);

  if (!status.Ok()) {
    return status;
  }

  state->title = channel.GetTitle();
  state->live = channel.GetLive();
  state->thumbnail = channel.GetThumbnail();
  state->viewers = channel.GetViewers();
  return status;
}

}  // namespace rustla2
