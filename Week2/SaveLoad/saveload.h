#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

template <typename T>
void Serialize(T pod, std::ostream& out);
void Serialize(const std::string& str, std::ostream& out);
template <typename T>
void Serialize(const std::vector<T>& data, std::ostream& out);
template <typename T1, typename T2>
void Serialize(const std::map<T1, T2>& data, std::ostream& out);

template <typename T1, typename T2>
void Deserialize(std::istream& in, std::map<T1, T2>& data);
void Deserialize(std::istream& in, std::string& str);
template <typename T>
void Deserialize(std::istream& in, std::vector<T>& data);
template <typename T>
void Deserialize(std::istream& in, T& pod);

// Serialization

template <typename T>
void Serialize(T pod, std::ostream& out) {
  out.write(reinterpret_cast<const char*>(&pod), sizeof(T));
}

void Serialize(const std::string& str, std::ostream& out) {
  auto size = str.size();
  out.write(reinterpret_cast<const char*>(&size), sizeof(size));
  out.write(str.data(), str.size());
}

template <typename T>
void Serialize(const std::vector<T>& data, std::ostream& out) {
  auto size = data.size();
  out.write(reinterpret_cast<const char*>(&size), sizeof(size));
  for (auto& item : data) {
    Serialize(item, out);
  }
}

template <typename T1, typename T2>
void Serialize(const std::map<T1, T2>& data, std::ostream& out) {
  auto size = data.size();
  out.write(reinterpret_cast<const char*>(&size), sizeof(size));
  for (auto& item : data) {
    Serialize(item.first, out);
    Serialize(item.second, out);
  }
}


// Deserialization

template <typename T>
void Deserialize(std::istream& in, T& pod) {
  in.read(reinterpret_cast<char*>(&pod), sizeof(T));
}

void Deserialize(std::istream& in, std::string& str) {
  size_t size;
  in.read(reinterpret_cast<char*>(&size), sizeof(size));
  str.resize(size);
  in.read(str.data(), str.size());
}

template <typename T>
void Deserialize(std::istream& in, std::vector<T>& data) {
  size_t size;
  in.read(reinterpret_cast<char*>(&size), sizeof(size));
  data.resize(size);
  for (auto& item : data) {
    Deserialize(in, item);
  }
}

template <typename T1, typename T2>
void Deserialize(std::istream& in, std::map<T1, T2>& data) {
  size_t size;
  in.read(reinterpret_cast<char*>(&size), sizeof(size));
  data.clear();
  T1 key;
  T2 value;
  for (size_t i = 0; i < size; ++i) {
    Deserialize(in, key);
    Deserialize(in, value);
    data.template emplace_hint(data.end(), std::move(key), std::move(value));
  }
}
