#include "HTTPResponseWriter.h"

#include <folly/String.h>
#include <boost/filesystem.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <fstream>
#include <locale>
#include <vector>

#include "Config.h"
#include "MIMETypes.h"
#include "Session.h"

namespace rustla2 {

const size_t kResponseGzipMinSize = 300;

namespace io = boost::iostreams;

void HTTPResponseWriter::Status(const uint32_t code, const std::string& label) {
  res_ << "HTTP/1.1 " << code << " " << label << "\r\n"
       << "Connection: close\r\n";
}

void HTTPResponseWriter::Header(const std::string& name, const std::tm* value) {
  res_ << name << ": ";
  std::string fmt = "%a, %d %b %Y %H:%M:%S %Z";
  std::use_facet<std::time_put<char>>(res_.getloc())
      .put(std::ostreambuf_iterator<char>(res_), res_, ' ', value, &fmt[0],
           &fmt[0] + fmt.size());
  res_ << "\r\n";
}

void HTTPResponseWriter::Header(const std::string& name,
                                const std::string& value) {
  res_ << name << ": " << value << "\r\n";
}

void HTTPResponseWriter::Header(const std::string& name, const int64_t value) {
  res_ << name << ": " << value << "\r\n";
}

void HTTPResponseWriter::Cookie(const std::string& name,
                                const std::string& value,
                                const std::string& domain, const time_t max_age,
                                const bool http_only, const bool secure) {
  res_ << "Set-Cookie: " << name << "=" << value;
  if (domain != "") {
    res_ << "; Domain=" << domain;
  }
  if (max_age != 0) {
    res_ << "; Max-Age=" << max_age;
  }
  if (http_only) {
    res_ << "; HttpOnly";
  }
  if (secure) {
    res_ << "; Secure";
  }
  res_ << "\r\n";
}

void HTTPResponseWriter::SessionCookie(const std::string& id) {
  Cookie(Config::Get().GetJWTName(), EncodeSessionCookie(id),
         Config::Get().GetJWTDomain(), Config::Get().GetJWTTTL(),
         false /* http_only */, Config::Get().GetJWTSecure());
}

void HTTPResponseWriter::Body(const char* body, const size_t size) {
  if (size < kResponseGzipMinSize) {
    Header("Content-Length", size);
    res_ << "\r\n" << folly::StringPiece(body, size) << std::flush;
    return;
  }

  io::filtering_streambuf<io::input> filter;
  filter.push(io::gzip_compressor());
  filter.push(io::array_source{body, size});

  std::string buf;
  io::copy(filter, io::back_insert_device<std::string>{buf});

  Header("Content-Encoding", "gzip");
  Header("Content-Length", buf.size());
  res_ << "\r\n" << buf << std::flush;
}

void HTTPResponseWriter::JSON(const std::string& data) {
  Header("Content-Type", "application/json");
  Body(data.data(), data.size());
}

void HTTPResponseWriter::LocalFile(const boost::filesystem::path& path) {
  std::string abs_path = boost::filesystem::absolute(path).string();

  time_t mtime = boost::filesystem::last_write_time(abs_path);
  Header("Last-Modified", std::localtime(&mtime));

  MIMETypes mime_types;
  Header("Content-Type", mime_types.Get(abs_path));

  std::ifstream file(abs_path, std::ios::binary);
  std::vector<char> buf;
  buf.assign(std::istreambuf_iterator<char>(file),
             std::istreambuf_iterator<char>());
  Body(reinterpret_cast<char*>(&buf.front()), buf.size());
}

}  // namespace rustla2
