#pragma once

#include <string>

namespace rustla2 {

std::string EncodeSessionCookie(const std::string& id);

std::string DecodeSessionCookie(const std::string& cookie);

}  // namespace rustla2
