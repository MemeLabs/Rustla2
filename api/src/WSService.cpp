#include "WSService.h"

#include <chrono>

#include "Config.h"
#include "HTTPRequest.h"
#include "JSON.h"

namespace rustla2 {

WSService::WSService(std::shared_ptr<DB> db, uWS::Hub* hub)
    : db_(db),
      hub_(hub),
      stream_broadcast_timer_(hub->getLoop()),
      rustler_broadcast_timer_(hub->getLoop()),
      stream_observer_(db_->GetStreams()->CreateObserver()) {
  // Run separate stream/rustler broadcast loops for each hub so we don't
  // need to use thread safe queues.
  stream_broadcast_timer_.setData(this);
  stream_broadcast_timer_.start(
      [](Timer* timer) {
        auto* websocket = static_cast<WSService*>(timer->getData());
        websocket->BroadcastStreams();
      },
      0, Config::Get().GetStreamBroadcastInterval());

  rustler_broadcast_timer_.setData(this);
  rustler_broadcast_timer_.start(
      [](Timer* timer) {
        auto* websocket = static_cast<WSService*>(timer->getData());
        websocket->BroadcastRustlers();
      },
      0, Config::Get().GetRustlerBroadcastInterval());

  hub->onConnection(
      [&](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest uws_req) {
        HTTPRequest req(uws_req);

        ws->send(last_streams_json_.data(), last_streams_json_.size(),
                 uWS::OpCode::TEXT);

        auto state = new WSState();
        state->id = boost::uuids::random_generator()();
        state->user_id = req.GetSessionID();
        state->ip = req.GetClientIPHeader().str();

        db_->GetViewerStates()->IncrViewerStream(state->user_id, 0);

        LOG(INFO) << "CONN_OPEN ws_id:" << state->id << " "
                  << "user_id:" << state->user_id << " "
                  << "ip : " << state->ip;

        ws->setUserData(reinterpret_cast<void*>(state));

        if (db_->GetBannedIPs()->Contains(req.GetClientIPHeader())) {
          ws->terminate();
        }
      });

  hub->onMessage([&](uWS::WebSocket<uWS::SERVER>* ws, char* message,
                     size_t length, uWS::OpCode opCode) {
    if (length == 0 || opCode != uWS::OpCode::TEXT) {
      return;
    }

    rapidjson::Document input;
    input.Parse(message, length);
    if (input.HasParseError() || !input.IsArray() || input.Size() == 0) {
      return;
    }

    const auto& command = input.GetArray();
    if (!command[0].IsString()) {
      return;
    }

    const json::StringRef method(command[0]);

    if (method == "setAfk") {
      SetAFK(ws, input);
    } else if (method == "setStream") {
      SetStream(ws, input);
    } else if (method == "getStream") {
      GetStream(ws, input);
    }
  });

  hub->onDisconnection([&](uWS::WebSocket<uWS::SERVER>* ws, int code,
                           char* message, size_t length) {
    UnsetStream(ws);

    auto state = reinterpret_cast<WSState*>(ws->getUserData());
    LOG(INFO) << "CONN_CLOSE ws_id:" << state->id
              << " user_id:" << state->user_id;
    delete state;
  });
}

WSService::~WSService() {
  stream_broadcast_timer_.stop();
  stream_broadcast_timer_.close();
  rustler_broadcast_timer_.stop();
  rustler_broadcast_timer_.close();
}

/**
 * Update rustler afk state.
 */
void WSService::SetAFK(uWS::WebSocket<uWS::SERVER>* ws,
                       const rapidjson::Document& input) {
  buf_.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf_);
  writer.StartArray();

  const auto& command = input.GetArray();
  if (command.Size() == 2 && command[1].IsBool()) {
    SetAFK(ws, command[1].GetBool(), &writer);
  } else {
    writer.String("ERR");
    writer.String("Invalid command");
  }

  writer.EndArray();
  ws->send(buf_.GetString(), buf_.GetSize(), uWS::OpCode::TEXT);
}

void WSService::SetAFK(uWS::WebSocket<uWS::SERVER>* ws, const bool afk,
                       rapidjson::Writer<rapidjson::StringBuffer>* writer) {
  auto ws_state = GetWSState(ws);

  if (ws_state->afk == afk) {
    writer->String("ERR");
    writer->String("AFK state unchanged");
    return;
  }

  auto stream = GetWSStream(ws);
  if (stream == nullptr) {
    writer->String("ERR");
    writer->String("Not viewing stream");
    return;
  }

  if (afk) {
    stream->IncrAFKCount();
  } else {
    stream->DecrAFKCount();
  }
  ws_state->afk = afk;

  writer->String("AFK_SET");
  writer->Bool(afk);
}

/**
 * Get a single stream definition for the supplied stream id... this (and all
 * non-broadcast sync operations) should be avoided but it's included for
 * compatability.
 */
void WSService::GetStream(uWS::WebSocket<uWS::SERVER>* ws,
                          const rapidjson::Document& input) {
  buf_.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf_);
  writer.StartArray();

  const auto& command = input.GetArray();
  if (command.Size() == 2 && command[1].IsUint64()) {
    GetStreamByID(command[1].GetUint64(), &writer);
  } else {
    writer.String("ERR");
    writer.String("Invalid command");
  }

  writer.EndArray();
  ws->send(buf_.GetString(), buf_.GetSize(), uWS::OpCode::TEXT);
}

/**
 * Handle requests for streams by id
 */
void WSService::GetStreamByID(
    const uint64_t stream_id,
    rapidjson::Writer<rapidjson::StringBuffer>* writer) {
  auto stream = db_->GetStreams()->GetByID(stream_id);
  if (stream == nullptr) {
    writer->String("ERR");
    writer->String("Invalid stream id");
    return;
  }

  writer->String("STREAM_GET");
  stream->WriteJSON(writer);
}

/**
 * Clear the current stream defined for the client. Dispatch the command to
 * the appropriate handler based on the json payload shape.
 *
 * If two strings are given treat them as channel and service names.
 * ex: ["setStream", "angelthump", "dariusirl"]
 *
 * If one string is given treat it as an overrustle user id.
 * ex: ["setStream", "dariusirl"]
 *
 * If a null literal is given ack without setting a stream
 * ex: ["setStream", null]
 */
void WSService::SetStream(uWS::WebSocket<uWS::SERVER>* ws,
                          const rapidjson::Document& input) {
  UnsetStream(ws);

  buf_.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf_);
  writer.StartArray();

  const auto& command = input.GetArray();
  uint64_t stream_id = 0;

  if (command.Size() == 3 && command[1].IsString() && command[2].IsString()) {
    // handle ["setStream", "channel", "service"]

    const json::StringRef channel(command[1]);
    const json::StringRef service(command[2]);
    SetStreamToChannel(channel, service, &writer, &stream_id);
  } else if (command.Size() == 2 && command[1].IsString()) {
    // handle ["setStream", "stream_path"]

    const json::StringRef stream_path(command[1]);
    SetStreamToStreamPath(stream_path, &writer, &stream_id);
  } else if (command.Size() == 2 && command[1].IsNull()) {
    // handle ["setStream", null]

    SetStreamToNull(&writer, &stream_id);
  } else {
    writer.String("ERR");
    writer.String("Invalid command");
  }

  writer.EndArray();
  ws->send(buf_.GetString(), buf_.GetSize(), uWS::OpCode::TEXT);

  auto state = GetWSState(ws);
  state->stream_id = stream_id;

  db_->GetViewerStates()->IncrViewerStream(state->user_id, stream_id);

  if (stream_id == 0) {
    LOG(INFO) << "STREAM_CLOSE ws_id:" << state->id << " "
              << "user_id:" << state->user_id;
    return;
  }

  const auto stream = db_->GetStreams()->GetByID(stream_id);

  stream->GetViewerIPs()->Insert(state->ip);

  const auto stream_channel = stream->GetChannel();
  LOG(INFO) << "STREAM_OPEN ws_id:" << state->id << " "
            << "user_id:" << state->user_id << " "
            << "stream_service:" << stream_channel->GetService() << " "
            << "stream_channel:" << stream_channel->GetChannel();
}

/**
 * Handle request for stream by channel/service
 */
inline void WSService::SetStreamToChannel(
    const std::string& channel, const std::string& service,
    rapidjson::Writer<rapidjson::StringBuffer>* writer, uint64_t* stream_id) {
  Status status;
  auto stream_channel = Channel::Create(channel, service, &status);
  if (!status.Ok()) {
    writer->String("ERR");
    writer->String(status.GetErrorMessage().c_str());
    return;
  }

  SetStreamToChannel(stream_channel, writer, stream_id);
}

/**
 * Handle request for stream by overrustle vanity url
 */
void WSService::SetStreamToStreamPath(
    const std::string& stream_path,
    rapidjson::Writer<rapidjson::StringBuffer>* writer, uint64_t* stream_id) {
  auto user = db_->GetUsers()->GetByStreamPath(stream_path);
  if (user == nullptr) {
    writer->String("ERR");
    writer->String("Invalid OverRustle ID");
    return;
  }

  auto channel = user->GetChannel();
  if (channel->IsEmpty()) {
    writer->String("ERR");
    writer->String("OverRustle user has no channel configured");
    return;
  }

  SetStreamToChannel(*channel, writer, stream_id);
}

/**
 * Handle stream upsert for all stream requests.
 *
 * TODO: this should probably be the model's responsibility...
 */
void WSService::SetStreamToChannel(
    const Channel& channel, rapidjson::Writer<rapidjson::StringBuffer>* writer,
    uint64_t* stream_id) {
  if (db_->GetBannedStreams()->Contains(channel)) {
    writer->String("STREAM_BANNED");
    writer->Null();
    return;
  }

  auto stream = db_->GetStreams()->GetByChannel(channel);
  if (stream == nullptr) {
    stream = db_->GetStreams()->Emplace(channel);
  }

  writer->String("STREAM_SET");
  stream->WriteJSON(writer);
  *stream_id = stream->GetID();
  stream->IncrRustlerCount();
}

/**
 * Handle return to stream index
 */
void WSService::SetStreamToNull(
    rapidjson::Writer<rapidjson::StringBuffer>* writer, uint64_t* stream_id) {
  writer->String("STREAM_SET");
  writer->Null();
}

/**
 * Clear the stream id associated with the supplied client
 */
void WSService::UnsetStream(uWS::WebSocket<uWS::SERVER>* ws) {
  auto stream = GetWSStream(ws);
  auto state = GetWSState(ws);

  if (stream != nullptr) {
    stream->DecrRustlerCount(state->afk);
    stream->GetViewerIPs()->Remove(state->ip);
  }

  db_->GetViewerStates()->DecrViewerStream(state->user_id, state->stream_id);
  state->stream_id = 0;
  state->afk = false;
}

/**
 * Generate STREAMS_SET broadcasts. Syncs updates from upstream services
 * ie. liveness, thumbnail, and viewer count.
 */
void WSService::BroadcastStreams() {
  buf_.Clear();
  rapidjson::Writer<rapidjson::StringBuffer> writer(buf_);

  writer.StartArray();
  writer.String("STREAMS_SET");
  db_->GetStreams()->WriteStreamsJSON(&writer);
  writer.EndArray();

  // if the list hasn't changed don't rebroadcast it
  if (last_streams_json_.compare(buf_.GetString()) != 0) {
    hub_->getDefaultGroup<uWS::SERVER>().broadcast(
        buf_.GetString(), buf_.GetSize(), uWS::OpCode::TEXT);

    last_streams_json_.assign(buf_.GetString(), buf_.GetSize());
  }
}

/**
 * Generate RUSTLERS_SET broadcasts for any streams whose rustler counts
 * changed recently. Handling this in a polling loop reduces the cost of
 * syncing clients by debouncing updates and creates a knob for load shedding.
 */
void WSService::BroadcastRustlers() {
  auto now = std::chrono::steady_clock::now();
  auto refresh_ivl =
      std::chrono::milliseconds(Config::Get().GetRustlerBroadcastInterval());
  auto last_rustler_broadcast_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          (now - refresh_ivl).time_since_epoch())
          .count();

  auto streams = db_->GetStreams();
  uint64_t id;
  while (stream_observer_->Next(&id)) {
    auto stream = streams->GetByID(id);
    if (stream->GetRemoved()) {
      continue;
    }

    buf_.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf_);
    writer.StartArray();

    // If the stream was reset since the last RUSTLERS_SET broadcast it's
    // a safe bet clients haven't received it via STREAMS_SET. Rather than
    // broadcasting the id and triggering a flood of `getStream` requests
    // broadcast the change as a STREAM_GET.
    if (stream->GetResetTime() >= last_rustler_broadcast_time) {
      writer.String("STREAM_GET");
      stream->WriteJSON(&writer);
    } else {
      writer.String("RUSTLERS_SET");
      writer.Uint64(stream->GetID());
      writer.Uint64(stream->GetRustlerCount());
      writer.Uint64(stream->GetAFKCount());
    }

    writer.EndArray();
    hub_->getDefaultGroup<uWS::SERVER>().broadcast(
        buf_.GetString(), buf_.GetSize(), uWS::OpCode::TEXT);
  }
}

std::shared_ptr<Stream> WSService::GetWSStream(
    uWS::WebSocket<uWS::SERVER>* ws) {
  auto ws_state = GetWSState(ws);
  if (ws_state->stream_id == 0) {
    return nullptr;
  }
  return db_->GetStreams()->GetByID(ws_state->stream_id);
}

WSState* WSService::GetWSState(uWS::WebSocket<uWS::SERVER>* ws) {
  return reinterpret_cast<WSState*>(ws->getUserData());
}

}  // namespace rustla2
