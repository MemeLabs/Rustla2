#pragma once

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <uWS/uWS.h>
#include <memory>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "Channel.h"
#include "DB.h"
#include "HTTPRequest.h"
#include "Streams.h"

namespace rustla2 {

struct WSState {
  boost::uuids::uuid id;
  std::string user_id{""};
  uint64_t stream_id{0};
  bool afk{false};
};

class WSService {
 public:
  WSService(std::shared_ptr<DB> db, uWS::Hub* hub);

  ~WSService();

  bool RejectBannedIP(uWS::WebSocket<uWS::SERVER>* ws, HTTPRequest& req);

  void SetAFK(uWS::WebSocket<uWS::SERVER>* ws,
              const rapidjson::Document& input);

  void SetAFK(uWS::WebSocket<uWS::SERVER>* ws, const bool afk,
              rapidjson::Writer<rapidjson::StringBuffer>* writer);

  void GetStream(uWS::WebSocket<uWS::SERVER>* ws,
                 const rapidjson::Document& input);

  void GetStreamByID(const uint64_t stream_id,
                     rapidjson::Writer<rapidjson::StringBuffer>* writer);

  void SetStream(uWS::WebSocket<uWS::SERVER>* ws,
                 const rapidjson::Document& input);

  inline void SetStreamToChannel(
      const std::string& channel, const std::string& service,
      rapidjson::Writer<rapidjson::StringBuffer>* writer, uint64_t* stream_id);

  void SetStreamToStreamPath(const std::string& stream_path,
                             rapidjson::Writer<rapidjson::StringBuffer>* writer,
                             uint64_t* stream_id);

  void SetStreamToChannel(const Channel& channel,
                          rapidjson::Writer<rapidjson::StringBuffer>* writer,
                          uint64_t* stream_id);

  void SetStreamToNull(rapidjson::Writer<rapidjson::StringBuffer>* writer,
                       uint64_t* stream_id);

  void UnsetStream(uWS::WebSocket<uWS::SERVER>* ws);

  void BroadcastStreams();

  void BroadcastRustlers();

  void BroadcastViewerState();

  std::shared_ptr<Stream> GetWSStream(uWS::WebSocket<uWS::SERVER>* ws);

  WSState* GetWSState(uWS::WebSocket<uWS::SERVER>* ws);

 private:
  std::shared_ptr<DB> db_;
  uWS::Hub* hub_;
  Timer stream_broadcast_timer_;
  Timer rustler_broadcast_timer_;
  rapidjson::StringBuffer buf_;
  uint64_t last_rustler_broadcast_time_{0};
  std::string last_streams_json_;
};

}  // namespace rustla2
