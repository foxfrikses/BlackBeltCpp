#pragma once

#include <string>
#include <string_view>

namespace string_stringview {
  class Hash {
    std::hash<std::string> sHash;
    std::hash<std::string_view> svHash;
  public:
    typedef void is_transparent;

    size_t operator()(const std::string& s1) const {
      return sHash(s1);
    }
    size_t operator()(std::string_view sv1) const {
      return svHash(sv1);
    }
  };
}