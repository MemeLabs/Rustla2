#include "HTTPRequest.h"

#include <boost/regex.hpp>

#include "Config.h"
#include "Session.h"

namespace rustla2 {

namespace {
const boost::regex query_regex("(?:^|&)([^=]+)=([^&]+)");
}

HTTPRequest::HTTPRequest(uWS::HttpRequest req) : req_(req) {
  const auto& url = req.getUrl();
  folly::StringPiece uri(url.value, url.valueLength);
  folly::StringPiece path, query;
  if (folly::split('?', uri, path, query)) {
    folly::split('/', path, path_);
    query_ = query;
  } else {
    folly::split('/', uri, path_);
  }
}

HTTPRequest::HTTPRequest(rustla2::HTTPRequest&& req) noexcept
    : req_(std::move(req.req_)),
      path_(std::move(req.path_)),
      query_(std::move(req.query_)),
      post_data_(std::move(req.post_data_)),
      post_data_handler_(std::move(req.post_data_handler_)) {}

void HTTPRequest::WritePostData(char* data, size_t length,
                                size_t remaining_bytes) {
  post_data_.append(data, length);

  if (remaining_bytes == 0 && post_data_handler_) {
    post_data_handler_(post_data_.data(), post_data_.size());
  }
}

const std::map<std::string, std::string> HTTPRequest::GetQueryParams() const {
  std::map<std::string, std::string> params;
  boost::cregex_iterator front(query_.begin(), query_.end(), query_regex);
  boost::cregex_iterator back;
  for (auto i = front; i != back; ++i) {
    params.insert({std::string((*i)[1].first, (*i)[1].second),
                   std::string((*i)[2].first, (*i)[2].second)});
  }
  return params;
}

std::string HTTPRequest::GetCookie(const std::string& name) {
  auto header = req_.getHeader("cookie");
  const folly::StringPiece cookie(header.value, header.valueLength);

  const std::string prefix = name + "=";
  size_t offset = cookie.find(prefix);
  if (offset == std::string::npos) {
    return "";
  }
  size_t start = offset + prefix.size();

  size_t end = cookie.find(";", start);
  if (end == std::string::npos) {
    end = cookie.size();
  }

  return cookie.subpiece(start, end - start).toString();
}

std::string HTTPRequest::GetSessionID() {
  auto cookie = GetCookie(Config::Get().GetJWTName());
  return DecodeSessionCookie(cookie);
}

folly::StringPiece HTTPRequest::GetClientIPHeader() {
  auto header = req_.getHeader(Config::Get().GetIPAddressHeader().c_str());
  return folly::StringPiece(header.value, header.valueLength);
}

}  // namespace rustla2
