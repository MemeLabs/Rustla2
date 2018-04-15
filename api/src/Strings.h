#pragma once

#include <string>

namespace rustla2 {

// copied from wikipedia. see:
// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C++
int levenshtein_distance(const std::string &s1, const std::string &s2);

}  // namespace rustla2
