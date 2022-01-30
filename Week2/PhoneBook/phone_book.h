#pragma once

#include "iterator_range.h"
#include "string_stringview_hash.h"
#include "string_stringview_equal.h"

#include <unordered_map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <iosfwd>
#include <iterator>

struct Date {
  int year, month, day;
};

struct Contact {
  std::string name;
  std::optional<Date> birthday;
  std::vector<std::string> phones;
};

class PhoneBook {
public:
  using Contacts = std::vector<Contact>;
  using ContactRange = IteratorRange<Contacts::const_iterator>;

  explicit PhoneBook(Contacts contacts);

  PhoneBook(PhoneBook&&) = default;
  PhoneBook& operator=(PhoneBook&&) = default;

  PhoneBook(const PhoneBook&) = delete;
  PhoneBook& operator=(const PhoneBook&) = delete;

  ContactRange FindByNamePrefix(std::string_view name_prefix) const;

  void SaveTo(std::ostream& output) const;

  friend PhoneBook DeserializePhoneBook(std::istream& input);

private:
  PhoneBook(Contacts sorted_contacts, int);

  Contacts sorted_contacts_;
  mutable std::unordered_map<std::string, ContactRange,
                             string_stringview::Hash,
                             string_stringview::Equal> cache_;
};

PhoneBook DeserializePhoneBook(std::istream& input);

