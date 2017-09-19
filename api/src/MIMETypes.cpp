#include "MIMETypes.h"

#include <boost/filesystem.hpp>

namespace rustla2 {

MIMETypes::MIMETypes() {
  magic_ = magic_open(MAGIC_MIME_TYPE);
  magic_load(magic_, nullptr);
  magic_compile(magic_, nullptr);

  for (const auto& t : mime_types) {
    mime_map_[t.extension.toString()] = t.name;
  }
}

MIMETypes::~MIMETypes() {
  magic_close(magic_);
  magic_ = nullptr;
}

std::string MIMETypes::Get(const std::string& path) {
  auto i = mime_map_.find(boost::filesystem::extension(path));
  auto mime =
      i != mime_map_.end() ? i->second : magic_file(magic_, path.c_str());

  return mime.toString();
}

}  // namespace rustla2
