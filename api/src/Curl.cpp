#include "Curl.h"

#include <cstring>

namespace rustla2 {

CurlRequest::CurlRequest(const std::string &url) : headers_(nullptr) {
  curl_ = curl_easy_init();
  if (curl_ == nullptr) {
    error_code_ = CURLE_FAILED_INIT;
    return;
  }
  error_code_ = CURLE_OK;

  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 3);
  curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 3);
  curl_easy_setopt(curl_, CURLOPT_ACCEPT_ENCODING, "");
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, CurlRequest::WriteCallback);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_data_);
}

CurlRequest::~CurlRequest() {
  if (curl_) {
    curl_easy_cleanup(curl_);
  }
}

void CurlRequest::EnableDebug() {
  curl_easy_setopt(curl_, CURLOPT_VERBOSE, true);
}

void CurlRequest::AddHeader(const std::string &data) {
  headers_ = curl_slist_append(headers_, data.c_str());
}

void CurlRequest::SetPostData(std::string data) {
  SetPostData(data.c_str(), data.size());
}

void CurlRequest::SetPostData(const char *data, size_t size) {
  curl_easy_setopt(curl_, CURLOPT_POST, 1);
  curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, size);
  curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data);
}

bool CurlRequest::Submit() {
  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
  error_code_ = curl_easy_perform(curl_);
  curl_slist_free_all(headers_);

  return Ok();
}

bool CurlRequest::Ok() const { return error_code_ == CURLE_OK; }

bool CurlRequest::GetErrorCode() const { return error_code_; }

std::string CurlRequest::GetErrorMessage() const {
  const char *message = curl_easy_strerror(error_code_);
  return std::string(message, strlen(message));
}

std::string CurlRequest::GetResponse() const { return response_data_.str(); }

int64_t CurlRequest::GetResponseCode() const {
  int64_t code;
  curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &code);
  return code;
}

size_t CurlRequest::WriteCallback(char *src, size_t size, size_t nmemb,
                                  void *dst) {
  auto *data = (std::stringstream *)dst;
  data->write(src, size * nmemb);
  return size * nmemb;
}

}  // namespace rustla2
