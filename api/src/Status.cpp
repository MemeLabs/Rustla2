#include "Status.h"

namespace rustla2 {

const Status& Status::OK = Status(StatusCode::OK, "");
const Status& Status::ERROR = Status(StatusCode::ERROR, "");

void Status::WriteJSON(
    rapidjson::Writer<rapidjson::StringBuffer>* writer) const {
  writer->StartObject();
  writer->Key("code");
  writer->Int(code_);
  writer->Key("message");
  writer->String(error_message_);
  writer->Key("details");
  writer->String(error_details_);
  writer->EndObject();
}

}  // namespace rustla2
