#pragma once

#include <memory>
#include <string>

#include "APIClient.h"
#include "DB.h"
#include "TwitchClient.h"
#include "YoutubeClient.h"

namespace rustla2 {

struct ChannelState {
  ChannelState() : thumbnail(""), viewers(0), live(false) {}

  std::string thumbnail;
  uint64_t viewers;
  bool live;
};

class ServicePoller {
 public:
  explicit ServicePoller(std::shared_ptr<DB> db);

  void Run();

  const APIStatus CheckAngelThump(const std::string& name, ChannelState* state);

  const APIStatus CheckTwitchStream(const std::string& name,
                                    ChannelState* state);

  const APIStatus CheckTwitchVOD(const std::string& name, ChannelState* state);

  const APIStatus CheckYouTube(const std::string& name, ChannelState* state);

 private:
  std::shared_ptr<DB> db_;
  std::unique_ptr<twitch::Client> twitch_;
  std::unique_ptr<youtube::Client> youtube_;
};

}  // namespace rustla2
