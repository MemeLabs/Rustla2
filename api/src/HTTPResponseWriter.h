#pragma once

#include <uWS/uWS.h>
#include <boost/filesystem/path.hpp>
#include <ctime>
#include <ostream>
#include <sstream>
#include <string>

namespace rustla2 {

class WSHTTPResponseProxy : public std::stringbuf {
 public:
  explicit WSHTTPResponseProxy(uWS::HttpResponse* res) : res_(res) {}

  ~WSHTTPResponseProxy() {
    if (res_) res_->end();
  }

 protected:
  virtual int sync() override final {
    auto buf = str();
    res_->write(buf.data(), buf.size());
    str("");
    return 0;
  }

 private:
  uWS::HttpResponse* res_;
};

class HTTPResponseWriter {
 public:
  explicit HTTPResponseWriter(std::stringstream& res)
      : proxy_(nullptr), res_(res.rdbuf()) {}

  explicit HTTPResponseWriter(uWS::HttpResponse* res)
      : proxy_(res), res_(&proxy_) {}

  void Status(const uint32_t code, const std::string& label);

  void Header(const std::string& name, const std::tm* value);

  void Header(const std::string& name, const std::string& value);

  void Header(const std::string& name, const int64_t value);

  void Cookie(const std::string& name, const std::string& value,
              const std::string& domain = "", const time_t max_age = 0,
              const bool http_only = false, const bool secure = false);

  void SessionCookie(const std::string& id);

  void Body(const std::string& body = "") { Body(body.data(), body.size()); }

  void Body(const char* body, const size_t size);

  void JSON(const std::string& data);

  void LocalFile(const boost::filesystem::path& path);

 private:
  WSHTTPResponseProxy proxy_;
  std::ostream res_;
};

}  // namespace rustla2
