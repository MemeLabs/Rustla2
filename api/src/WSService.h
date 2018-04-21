#pragma once

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <uWS/uWS.h>
#include <memory>

#include "Channel.h"
#include "DB.h"

namespace rustla2 {

class WSService {
 public:
  WSService(std::shared_ptr<DB> db, uWS::Hub* hub);

  ~WSService();

  bool RejectBannedIP(uWS::WebSocket<uWS::SERVER>* ws,
                      uWS::HttpRequest uws_req);

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
