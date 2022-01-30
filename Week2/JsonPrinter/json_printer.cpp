#include "test_runner.h"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <stack>
#include <string>
#include <variant>

void PrintJsonString(std::ostream& out, std::string_view str) {
  out << "\"";
  bool afterSlash = false;
  for (auto c : str) {
    if (afterSlash) {
      out << '\\';
      if (c != '\\' && c != '"') {
        out << '\\';
      }
      else out << c;
      afterSlash = false;
    }
    else if (c == '"') {
      out << "\\\"";
    }
    else if (c == '\\') {
      afterSlash = true;
    }
    else {
      out << c;
    }
  }
  if (afterSlash) out << "\\\\";
  out << "\"";
}

namespace JsonContext {
  template <class ParentContext> class Object;
  template <class ParentContext> class Array;
  template <class ParentObject> class ObjectValue;

  static class Empty {} empty;

  template <class ParentObject>
  class ObjectValue {
    ParentObject& parentObject_;
    std::ostream& output_;

    bool isPrinted = false;

  public:
    ObjectValue(std::ostream& output, ParentObject& parentObject)
      : parentObject_(parentObject)
      , output_{output}
    {}
    ~ObjectValue() {
      if (!std::exchange(isPrinted, true)) {
        Null();
      }
    }
    ParentObject& Number(int64_t num) {
      isPrinted = true;
      output_ << num;
      return parentObject_;
    }
    ParentObject& String(std::string_view str) {
      isPrinted = true;
      PrintJsonString(output_, str);
      return parentObject_;
    }
    ParentObject& Boolean(bool b) {
      isPrinted = true;
      output_ << std::boolalpha << b;
      return parentObject_;
    }
    ParentObject& Null() {
      isPrinted = true;
      output_ << "null";
      return parentObject_;
    }
    Array<ParentObject> BeginArray() {
      isPrinted = true;
      return Array<ParentObject>(output_, parentObject_);
    }
    Object<ParentObject> BeginObject() {
      isPrinted = true;
      return Object<ParentObject>(output_, parentObject_);
    }
  };

  template <class ParentContext>
  class Object {
    using Self = Object;

    std::ostream& output_;
    ParentContext& parentContext_;

    bool isEmpty = true;
    bool isFinished = false;

  public:
    Object(std::ostream& output, ParentContext& parentContext)
      : output_(output)
      , parentContext_(parentContext)
    {
      output_ << '{';
    }

    ~Object() {
      EndObject();
    }

    ParentContext& EndObject() {
      if (!std::exchange(isFinished, true)) {
        output_ << '}';
      }
      return parentContext_;
    }

    ObjectValue<Self> Key(std::string_view str) {
      if (!std::exchange(isEmpty, false)) {
        output_ << ',';
      }
      PrintJsonString(output_, str);
      output_ << ':';
      return ObjectValue<Self>(output_, *this);
    }
  };

  template <class ParentContext>
  class Array {
    using Self = Array;

    std::ostream& output_;
    ParentContext&  parentContext_;

    bool isEmpty_ = true;
    bool isFinished_ = false;

    void BeforeValuePrint() {
      if (!std::exchange(isEmpty_, false)) {
        output_ << ',';
      }
    }

  public:
    Array(std::ostream& output, ParentContext& parentContext)
      : output_(output)
      , parentContext_(parentContext)
    {
      output_ << '[';
    }

    ~Array() {
      EndArray();
    }
    ParentContext& EndArray() {
      if (!std::exchange(isFinished_, true)) {
        output_ << ']';
      }
      return parentContext_;
    }
    Self& Number(int n) {
      BeforeValuePrint();
      output_ << n;
      return *this;
    }
    Self& String(std::string_view str) {
      BeforeValuePrint();
      PrintJsonString(output_, str);
      return *this;
    }
    Self& Boolean(bool b) {
      BeforeValuePrint();
      output_ << std::boolalpha << b;
      return *this;
    }
    Self& Null() {
      BeforeValuePrint();
      output_ << "null";
      return *this;
    }
    Array<Self> BeginArray() {
      BeforeValuePrint();
      return Array<Self>(output_, *this);
    }
    Object<Self> BeginObject() {
      BeforeValuePrint();
      return Object<Self>(output_, *this);
    }
  };
}

using ArrayContext = JsonContext::Array<JsonContext::Empty>;
ArrayContext PrintJsonArray(std::ostream& out) {
  return JsonContext::Array<JsonContext::Empty>(out, JsonContext::empty);
}

using ObjectContext = JsonContext::Object<JsonContext::Empty>;
ObjectContext PrintJsonObject(std::ostream& out) {
  return JsonContext::Object<JsonContext::Empty>(out, JsonContext::empty);
}

void TestArray() {
  std::ostringstream output;

  {
    auto json = PrintJsonArray(output);
    json
        .Number(5)
        .Number(6)
        .BeginArray()
        .Number(7)
        .EndArray()
        .Number(8)
        .String("bingo!");
  }

  ASSERT_EQUAL(output.str(), R"([5,6,[7],8,"bingo!"])");
}

void TestObject() {
  std::ostringstream output;

  {
    auto json = PrintJsonObject(output);
    json
        .Key("id1").Number(1234)
        .Key("id2").Boolean(false)
        .Key("").Null()
        .Key("\"").String("\\");
  }

  ASSERT_EQUAL(output.str(), R"({"id1":1234,"id2":false,"":null,"\"":"\\"})");
}

void TestAutoClose() {
  std::ostringstream output;

  {
    auto json = PrintJsonArray(output);
    json.BeginArray().BeginObject();
  }

  ASSERT_EQUAL(output.str(), R"([[{}]])");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestArray);
  RUN_TEST(tr, TestObject);
  RUN_TEST(tr, TestAutoClose);

  std::cout << std::endl;
  PrintJsonObject(std::cout)
    .Key("V")
      .BeginArray()
        .String("sgjh")
        .Null()
      .EndArray()
    .Key("fs")
      .BeginObject()
        .Key("hfe").Boolean(true)
      .EndObject();


  return 0;
}
