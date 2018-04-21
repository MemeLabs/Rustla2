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
      rustler_broadcast_timer_(hub->getLoop()) {
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

  hub->onConnection([&](uWS::WebSocket<uWS::SERVER>* ws, uWS::HttpRequest req) {
    if (RejectBannedIP(ws, req)) {
      return;
    }

    ws->send(last_streams_json_.data(), last_streams_json_.size(),
             uWS::OpCode::TEXT);
  });

  hub->onMessage([&](uWS::WebSocket<uWS::SERVER>* ws, char* message,
                     size_t length, uWS::OpCode opCode) {
    if (length == 0 || opCode != uWS::OpCode::TEXT) {
      return;
    }

    rapidjson::Document input;
    input.Parse(message, length);
    if (input.HasParseError() || !input.IsArray()) {
      return;
    }

    const auto& command = input.GetArray();
    if (!command[0].IsString()) {
      return;
    }

    const json::StringRef method(command[0]);

    if (method == "setStream") {
      SetStream(ws, input);
    } else if (method == "getStream") {
      GetStream(ws, input);
    }
  });

  hub->onDisconnection([&](uWS::WebSocket<uWS::SERVER>* ws, int code,
                           char* message, size_t length) { UnsetStream(ws); });
}

WSService::~WSService() {
  stream_broadcast_timer_.stop();
  stream_broadcast_timer_.close();
  rustler_broadcast_timer_.stop();
  rustler_broadcast_timer_.close();
}

bool WSService::RejectBannedIP(uWS::WebSocket<uWS::SERVER>* ws,
                               uWS::HttpRequest uws_req) {
  HTTPRequest req(uws_req);
  if (db_->GetBannedIPs()->Contains(req.GetClientIPHeader())) {
    ws->terminate();
    return true;
  }

  return false;
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
  ws->setUserData(reinterpret_cast<void*>(stream_id));
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
  auto stream_id = reinterpret_cast<uint64_t>(ws->getUserData());

  if (stream_id != 0) {
    auto stream = db_->GetStreams()->GetByID(stream_id);
    if (stream != nullptr) {
      stream->DecrRustlerCount();
    }
    ws->setUserData(reinterpret_cast<void*>(0));
  }
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
  auto last_rustler_broadcast_time =
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count();
  auto streams =
      db_->GetStreams()->GetAllUpdatedSince(last_rustler_broadcast_time_);

  for (const auto& stream : streams) {
    buf_.Clear();
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf_);
    writer.StartArray();

    // If the stream was reset since the last RUSTLERS_SET broadcast it's
    // a safe bet clients haven't received it via STREAMS_SET. Rather than
    // broadcasting the id and triggering a flood of `getStream` requests
    // broadcast the change as a STREAM_GET.
    if (stream->GetResetTime() >= last_rustler_broadcast_time_) {
      writer.String("STREAM_GET");
      stream->WriteJSON(&writer);
    } else {
      writer.String("RUSTLERS_SET");
      writer.Uint64(stream->GetID());
      writer.Uint64(stream->GetRustlerCount());
    }

    writer.EndArray();
    hub_->getDefaultGroup<uWS::SERVER>().broadcast(
        buf_.GetString(), buf_.GetSize(), uWS::OpCode::TEXT);
  }

  last_rustler_broadcast_time_ = last_rustler_broadcast_time;
}

}  // namespace rustla2
