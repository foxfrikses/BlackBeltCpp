#include <cstddef>
#include <utility>
#include <memory>
#include <algorithm>

template <typename T>
class VectorBuffer final {
  T* data = nullptr;
  size_t sz = 0;

  static T* Allocate(size_t size) {
    return reinterpret_cast<T*>(operator new (sizeof(T) * size));
  }
  static void Deallocate(T* data) {
    operator delete (reinterpret_cast<void*>(data));
  }

public:
  VectorBuffer() = default;
  VectorBuffer(size_t size)
    : data(Allocate(size))
    , sz(size)
  {}

  VectorBuffer(const VectorBuffer&) = delete;
  VectorBuffer& operator=(const VectorBuffer&) = delete;

  VectorBuffer(VectorBuffer&& other) noexcept
    : data(other.data)
    , sz(other.sz)
  {
    other.data = nullptr;
    other.sz = 0;
  }

  VectorBuffer& operator=(VectorBuffer&& other) noexcept {
    std::swap(other.data, data);
    std::swap(other.sz, sz);
    return *this;
  }

  auto Data() {return data;}
  auto Data() const {return data;}

  size_t Size() {return sz;}
  size_t Size() const {return sz;}

  T* operator +(size_t shift) {return data + shift;}
  const T* operator +(size_t shift) const {return data + shift;}

  T& operator[](size_t index) {return data[index];}
  const T& operator[](size_t index) const {return data[index];}

  ~VectorBuffer() {
    if (sz > 0) {
      Deallocate(data);
    }
  }
};


template <typename T>
class Vector {
  VectorBuffer<T> buffer;
  size_t sz = 0;

public:
  Vector() = default;
  Vector(size_t n)
    : buffer(n)
    , sz(n)
  {
    std::uninitialized_value_construct_n(buffer.Data(), sz);
  }
  Vector(const Vector& other)
    : buffer(other.sz)
    , sz(other.sz)
  {
    std::uninitialized_copy_n(other.buffer.Data(), sz, buffer.Data());
  }
  Vector(Vector&& other) noexcept
  {
    std::swap(other.buffer, buffer);
    std::swap(other.sz, sz);
  }

  ~Vector() {
    std::destroy_n(buffer.Data(), sz);
  }

  Vector& operator = (const Vector& other) {
    if (buffer.Size() < other.sz) {
      Vector newVector = other;
      *this = std::move(newVector);
      return *this;
    }

    std::copy_n(other.buffer.Data(), std::min(other.sz, sz), buffer.Data());
    if (other.sz > sz) {
      std::uninitialized_copy_n(other.buffer + sz, other.sz - sz, buffer + sz);
    }
    else if (other.sz < sz) {
      std::destroy_n(buffer + other.sz, sz - other.sz);
    }
    sz = other.sz;
    return *this;
  }
  Vector& operator = (Vector&& other) noexcept {
    std::swap(other.buffer, buffer);
    std::swap(other.sz, sz);
    return *this;
  }

  void Reserve(size_t n) {
    if (n > buffer.Size()) {
      VectorBuffer<T> newBuffer(n);
      std::uninitialized_move_n(buffer.Data(), sz, newBuffer.Data());
      std::swap(buffer, newBuffer);
      std::destroy_n(newBuffer.Data(), sz);
    }
  }

  void Resize(size_t n) {
    Reserve(n);
    if (n < sz) {
      std::destroy_n(buffer + n, sz - n);
    }
    else if (n > sz) {
      std::uninitialized_value_construct_n(buffer + sz, n - sz);
    }
    sz = n;
  }

  void PushBack(const T& elem) {
    if (sz == buffer.Size()) {
      Reserve(sz == 0 ? 1 : sz * 2);
    }
    //std::construct_at(buffer + sz, elem);
    new (buffer + sz) T(elem);
    ++sz;
  }
  void PushBack(T&& elem) {
    if (sz == buffer.Size()) {
      Reserve(sz == 0 ? 1 : sz * 2);
    }
    //std::construct_at(buffer + sz, std::move(elem));
    new (buffer + sz) T(std::move(elem));
    ++sz;
  }

  template <typename ... Args>
  T& EmplaceBack(Args&&... args) {
    if (sz == buffer.Size()) {
      Reserve(sz == 0 ? 1 : sz * 2);
    }
    //T* p = std::construct_at(buffer + sz, std::forward<Args>(args)...);
    T* p = new (buffer + sz) T(std::forward<Args>(args)...);
    ++sz;
    return *p;
  }

  void PopBack() {
    std::destroy_at(buffer + sz - 1);
    --sz;
  }

  size_t Size() const noexcept {return sz;}

  size_t Capacity() const noexcept {return buffer.Size();}

  const T& operator[](size_t i) const {return buffer[i];}
  T& operator[](size_t i) {return buffer[i];}
};
