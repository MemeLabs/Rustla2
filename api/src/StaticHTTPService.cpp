#include "StaticHTTPService.h"

#include <folly/String.h>
#include <uWS/uWS.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <sstream>

#include "HTTPRequest.h"
#include "HTTPResponseWriter.h"

namespace rustla2 {

StaticCacheEntry::StaticCacheEntry(const fs::path& path) {
  std::stringstream buf;
  HTTPResponseWriter writer(buf);
  writer.Status(200, "OK");
  writer.Header("Cache-Control", "max-age=3600, public");
  writer.LocalFile(path);

  data_ = buf.str();
  data_size_ = data_.size();

  const std::string header_delimiter = "\r\n\r\n";
  header_size_ = data_.find(header_delimiter) + header_delimiter.size();
}

StaticHTTPService::StaticHTTPService(const std::string& root_dir,
                                     const std::string& index) {
  for (fs::recursive_directory_iterator
           i = fs::recursive_directory_iterator(fs::path(root_dir)),
           end_iter;
       i != end_iter; i++) {
    auto path = i->path();
    if (fs::is_regular_file(path)) {
      folly::StringPiece server_path(path.string());
      server_path.removePrefix(root_dir);
      server_path.removeSuffix(index);

      cache_[server_path.toString()].reset(new StaticCacheEntry(path));
    }
  }
}

void StaticHTTPService::RegisterRoutes(HTTPRouter* router) {
  for (const auto& i : cache_) {
    const auto cache = i.second;
    router->Get(i.first, [=](uWS::HttpResponse* res, HTTPRequest* req) {
      res->write(cache->Data(), cache->Size());
      res->end();
    });

    router->Head(i.first, [=](uWS::HttpResponse* res, HTTPRequest* req) {
      res->write(cache->Data(), cache->HeaderSize());
      res->end();
    });
  }
}

void StaticHTTPService::ServeIndex(uWS::HttpResponse* res) {
  const auto& cache = cache_["/"];
  res->write(cache->Data(), cache->Size());
  res->end();
}

}  // namespace rustla2
