#include "StaticHTTPService.h"

#include <folly/String.h>
#include <glog/logging.h>
#include <uWS/uWS.h>

#include <sstream>

#include "HTTPRequest.h"
#include "HTTPResponseWriter.h"

namespace rustla2 {

const fs::path StaticCacheEntry::VOID_PATH = "";

StaticCacheEntry::StaticCacheEntry(const fs::path& path) {
  std::stringstream buf;
  HTTPResponseWriter writer(buf);
  writer.Status(200, "OK");
  writer.Header("Cache-Control", "max-age=3600, public");

  if (path == StaticCacheEntry::VOID_PATH) {
    writer.Body("", 0);
  } else {
    writer.LocalFile(path);
  }

  data_ = buf.str();
  data_size_ = data_.size();

  const std::string header_delimiter = "\r\n\r\n";
  header_size_ = data_.find(header_delimiter) + header_delimiter.size();
}

StaticHTTPService::StaticHTTPService(const std::string& root_dir,
                                     const std::string& index) {
  cache_["/"].reset(new StaticCacheEntry(StaticCacheEntry::VOID_PATH));

  if (!fs::is_directory(root_dir)) {
    LOG(ERROR) << "http server path does not exist (" << root_dir << ")";
    return;
  }

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
