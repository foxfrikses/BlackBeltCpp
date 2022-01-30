#include "phone_book.h"

#include "contact.pb.h"

using namespace std;
using Contacts = PhoneBook::Contacts;
using ContactRange = PhoneBook::ContactRange;

struct ContactNameCompare {
  typedef void is_transparent;
  bool operator()(const Contact &contact, string_view name) const {
    string_view cname(contact.name.c_str(), min(name.size(), contact.name.size()));
    return cname < name;
  }
  bool operator()(string_view name, const Contact &contact) const {
    string_view cname(contact.name.c_str(), min(name.size(), contact.name.size()));
    return name < cname;
  }
};

Contacts SortContacts(Contacts contacts) {
  std::sort(begin(contacts),
            end(contacts),
            [](const Contact &l,const Contact &r) {
                return l.name < r.name;
              }
            );
  return contacts;
}

PhoneBook::PhoneBook(Contacts sorted_contacts, int)
  : sorted_contacts_(move(sorted_contacts))
{}

PhoneBook::PhoneBook(Contacts contacts)
  : sorted_contacts_(SortContacts(move(contacts)))
{}

ContactRange PhoneBook::FindByNamePrefix(string_view name_prefix) const {
  if (auto found = cache_.find(name_prefix); found != cache_.end()) {
    return found->second;
  }

  auto [first, last] = equal_range(
    sorted_contacts_.begin(),
    sorted_contacts_.end(),
    name_prefix,
    ContactNameCompare()
  );

  return cache_.emplace(string(name_prefix), ContactRange(first, last)).first->second;
}

void PhoneBook::SaveTo(ostream& output) const {
  PhoneBookSerialize::ContactList contactList;

  for (auto& sContact : sorted_contacts_) {
    auto& contact = *contactList.add_contact();
    contact.set_name(sContact.name);
    if (sContact.birthday) {
      auto& birthday = *contact.mutable_birthday();
      birthday.set_year(sContact.birthday->year);
      birthday.set_month(sContact.birthday->month);
      birthday.set_day(sContact.birthday->day);
    }
    for (auto& phone : sContact.phones) {
      contact.mutable_phone_number()->Add(string(phone));
    }
  }

  contactList.SerializePartialToOstream(&output);
}

PhoneBook DeserializePhoneBook(istream& input) {
  PhoneBookSerialize::ContactList contactList;
  contactList.ParseFromIstream(&input);

  Contacts sortedContacts;
  sortedContacts.reserve(contactList.contact_size());

  for (auto& contact : contactList.contact()) {
    auto& sContact = sortedContacts.emplace_back();
    sContact.name = contact.name();
    if (contact.has_birthday()) {
      sContact.birthday = Date {
        .year = contact.birthday().year(),
        .month = contact.birthday().month(),
        .day = contact.birthday().day(),
      };
    }
    sContact.phones.reserve(contact.phone_number_size());
    for (auto& phone : contact.phone_number()) {
      sContact.phones.push_back(phone);
    }
  }

  return PhoneBook(move(sortedContacts), 0);
}
