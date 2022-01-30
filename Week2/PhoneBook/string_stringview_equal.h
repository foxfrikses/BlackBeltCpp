#pragma once

#include <string>
#include <string_view>

namespace string_stringview {
  class Equal {
  public:
    typedef void is_transparent;

    bool operator()(const std::string& s1, const std::string& s2) const {
      return s1 == s2;
    }
    bool operator()(const std::string& s, std::string_view sv) const {
      return s == sv;
    }
    bool operator()(std::string_view sv, const std::string& s) const {
      return sv == s;
    }
    bool operator()(std::string_view sv1, std::string_view sv2) const {
      return sv1 == sv2;
    }
  };
}

