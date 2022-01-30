#include "yellow_pages.h"

#include <algorithm>
#include <tuple>
#include <vector>
#include <string>
#include <limits>

using namespace std;

namespace YellowPages {
  void MergeValue(string field, const Signals& signals, const Providers& providers, Company& company) {
    auto fieldDescriptor = company.GetDescriptor()->FindFieldByName(field);
    auto reflection = company.GetReflection();
    string serializedField;
    int priority = numeric_limits<uint64_t>::min();
    for (auto& signal : signals) {
      if (!signal.has_company()) continue;
      if (!reflection->HasField(signal.company(), fieldDescriptor)) continue;
      auto curPriority = providers.at(signal.provider_id()).priority();
      if (priority > curPriority) continue;
      priority = curPriority;

      serializedField = reflection->GetMessage(signal.company(), fieldDescriptor).SerializeAsString();
    }
    if (serializedField.empty()) return;
    reflection->MutableMessage(&company, fieldDescriptor)->ParseFromString(serializedField);
  }

  void MergeRepValue(string field, const Signals& signals, const Providers& providers, Company& company) {
    auto fieldDescriptor = company.GetDescriptor()->FindFieldByName(field);
    auto reflection = company.GetReflection();
    unordered_set<string> serializedFields;
    int priority = numeric_limits<uint64_t>::min();
    for (auto& signal : signals) {
      if (!signal.has_company()) continue;
      auto size = reflection->FieldSize(signal.company(), fieldDescriptor);
      if (size == 0) continue;
      auto curPriority = providers.at(signal.provider_id()).priority();
      if (curPriority < priority) continue;
      if (priority < curPriority) {
        serializedFields.clear();
      }
      priority = curPriority;
      for (int i = 0; i < size; ++i) {
        serializedFields.insert(reflection->GetRepeatedMessage(signal.company(),
                                                                  fieldDescriptor,
                                                                  i).SerializeAsString());
      }
    }
    for (auto& serializedField : serializedFields) {
      reflection->AddMessage(&company, fieldDescriptor)->ParseFromString(serializedField);
    }
  }

  Company Merge(const Signals& signals, const Providers& providers) {
    Company company;
    MergeValue("working_time", signals, providers, company);
    MergeValue("address", signals, providers, company);
    MergeRepValue("names", signals, providers, company);
    MergeRepValue("phones", signals, providers, company);
    MergeRepValue("urls", signals, providers, company);
    return company;
  }
}
