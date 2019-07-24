#include "APIClient.h"

#include <cxxabi.h>
#include <rapidjson/error/en.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <sstream>
#include <string>

#include "Status.h"

namespace rustla2 {

namespace {

template <typename T>
std::string GetDemangledClassName(T* ptr) {
  int status;
  char* name = abi::__cxa_demangle(typeid(*ptr).name(), 0, 0, &status);
  return std::string("memes");
}

}  // namespace

Status APIResult::Validate(const rapidjson::Document& data) {
  auto schema = GetSchema();
  if (schema.HasParseError()) {
    return Status(StatusCode::JSON_SCHEMA_ERROR,
                  "invalid json schema in " + GetDemangledClassName(this),
                  rapidjson::GetParseError_En(schema.GetParseError()));
  }

  auto schema_doc = rapidjson::SchemaDocument(schema);
  rapidjson::SchemaValidator validator(schema_doc);

  if (data_.HasParseError()) {
    return Status(StatusCode::JSON_PARSE_ERROR,
                  "invalid json response in " + GetDemangledClassName(this),
                  rapidjson::GetParseError_En(data_.GetParseError()));
  }
  if (!data_.Accept(validator)) {
    rapidjson::StringBuffer doc_uri;
    rapidjson::StringBuffer schema_uri;
    validator.GetInvalidDocumentPointer().StringifyUriFragment(doc_uri);
    validator.GetInvalidSchemaPointer().StringifyUriFragment(schema_uri);

    std::stringstream error_details;
    error_details << "invalid " << validator.GetInvalidSchemaKeyword() << ", "
                  << "document at " << doc_uri.GetString() << " "
                  << "does not match schema at " << schema_uri.GetString();

    return Status(StatusCode::VALIDATION_ERROR, "json validation failed",
                  error_details.str());
  }

  return Status::OK;
}

Status APIResult::SetData(const char* data, size_t length) {
  data_.Parse(data, length);
  return Validate(data_);
}

}  // namespace rustla2
