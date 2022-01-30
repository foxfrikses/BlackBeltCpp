#pragma once

#include <stdexcept>

struct BadOptionalAccess : public std::exception {};

template <typename T>
class Optional {
private:
  alignas(T) unsigned char data[sizeof(T)];
  bool defined = false;

public:
  Optional() = default;
  Optional(const T& elem) {
    *this = elem;
  }
  Optional(T&& elem) {
    *this = std::move(elem);
  }
  Optional(const Optional& other) {
    *this = other;
  }
  Optional(Optional&& other) {
    *this = std::move(other);
  }

  Optional& operator=(const T& elem) {
    if (defined) {
      operator*() = elem;
    }
    else {
      new (data) T(elem);
      defined = true;
    }
    return *this;
  }
  Optional& operator=(T&& elem) {
    if (defined) {
      operator*() = std::move(elem);
    }
    else {
      new (data) T(std::move(elem));
      defined = true;
    }
    return *this;
  }
  Optional& operator=(const Optional& other) {
    if (other.defined && defined) {
      operator*() = *other;
    }
    else if (other.defined) {
      new (data) T(*reinterpret_cast<const T*>(other.data));
      defined = true;
    }
    else {
      Reset();
    }
    return *this;
  }
  Optional& operator=(Optional&& other) {
    if (other.defined && defined) {
      operator*() = std::move(other.operator*());
    }
    else if (other.defined) {
      new (data) T(std::move(other.operator*()));
      defined = true;
    }
    else {
      other.Reset();
    }
    return *this;
  }

  bool HasValue() const {return defined;}

  // Эти операторы не должны делать никаких проверок на пустоту.
  // Проверки остаются на совести программиста.
  T& operator*() {return *reinterpret_cast<T*>(data);}
  const T& operator*() const {return *reinterpret_cast<const T*>(data);}
  T* operator->() {return reinterpret_cast<T*>(data);}
  const T* operator->() const {return reinterpret_cast<const T*>(data);}

  // Генерирует исключение BadOptionalAccess, если объекта нет
  T& Value() {
    if (!defined) throw BadOptionalAccess();
    return operator*();
  }
  const T& Value() const {
    if (!defined) throw BadOptionalAccess();
    return operator*();
  }

  void Reset() {
    if (defined) {
      reinterpret_cast<T*>(data)->~T();
      defined = false;
    }
  }

  ~Optional() {
    Reset();
  }
};
