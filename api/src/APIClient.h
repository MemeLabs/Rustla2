#pragma once

#include <rapidjson/document.h>
#include <string>

#include "Curl.h"

namespace rustla2 {

enum StatusCode {
  UNKNOWN,
  OK,
  ERROR,
  HTTP_ERROR,
  JSON_PARSE_ERROR,
  JSON_SCHEMA_ERROR,
  JSON_VALIDATION_ERROR,
  API_ERROR
};

class APIStatus {
 public:
  APIStatus() : code_(StatusCode::UNKNOWN) {}

  APIStatus(StatusCode code, const std::string& error_message)
      : code_(code), error_message_(error_message) {}

  APIStatus(StatusCode code, const std::string& error_message,
            const std::string& error_details)
      : code_(code),
        error_message_(error_message),
        error_details_(error_details) {}

  static const APIStatus& OK;
  static const APIStatus& ERROR;

  bool Ok() const { return code_ == StatusCode::OK; }

  std::string GetErrorMessage() const { return error_message_; }

  std::string GetErrorDetails() const { return error_details_; }

 private:
  StatusCode code_;
  std::string error_message_;
  std::string error_details_;
};

class APIResult {
 public:
  virtual ~APIResult() = default;

  const rapidjson::Document& GetData() const { return data_; }

  virtual rapidjson::Document GetSchema() = 0;

  virtual APIStatus Validate(const rapidjson::Document& data);

  APIStatus SetData(const char* data, size_t length);

 private:
  rapidjson::Document data_;
};

}  // namespace rustla2
