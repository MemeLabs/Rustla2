#pragma once

#include <curl/curl.h>
#include <cstdlib>
#include <sstream>
#include <string>

namespace rustla2 {

class CurlRequest {
 public:
  explicit CurlRequest(const std::string &url);

  ~CurlRequest();

  void EnableDebug();

  void AddHeader(const std::string &data);

  void SetPostData(std::string data);

  void SetPostData(const char *data, size_t size);

  bool Submit();

  bool Ok() const;

  bool GetErrorCode() const;

  std::string GetErrorMessage() const;

  std::string GetResponse() const;

  int64_t GetResponseCode() const;

  static size_t WriteCallback(char *src, size_t size, size_t nmemb, void *dst);

 private:
  CURL *curl_;
  curl_slist *headers_;
  std::stringstream response_data_;
  CURLcode error_code_;
};

}  // namespace rustla2
