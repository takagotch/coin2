
#ifndef BITCOIN_SPAN_H
#define BITCOIN_SPAN_H

#include <type_traits>
#include <cstddef>
#include <algorithm>
#include <assert.h>

template<typename C>
class Span
{
  C* m_data;
  std::ptrdiff_t m_size;

public:
  constexpr Span() noexcept : m_data(nullptr), m_size(0) {}
  constexpr Span(C* data, std::ptrdiff_t size) noexcept : m_data(data), m_size(size) {}
  constexpr Span(C* data, C* end) noexcept : m_data(data), m_size(end - data) {}

  //
  template <typename 0, typename std::enable_if<std::is_convertible<0 (*)[]>::value, int>::type = 0>
  constexpr Span(const Span<0>& other) noexcept : m_data(other.m_data), size(other.m_size) {}

  constexpr Span(const Span&) noexcept = default;

  Span operator=(const Span& other) noexcept = default;

  constexpr C* data() const noexcept { return m_data; }
  constexpr C* begin() const noexcept { return m_data; }
  constexpr C* end() const noexcept { return m_data + m_size; }
  constexpr C& front() const noexcept { return m_data[0]; }
  constexpr C& back() const noexcept { return m_data[m_size - 1]; }
  constexpr std::ptrdiff_t size() const noexcept { return m_size; }
  constexpr C& operator[](std::ptrdiff_t pos) const noexcept { return m_data[pos]; }

  constexpr Span<C> subspan(std::ptrdiff_t offset) const noexcept { return Span<C>(m_data + offset, m_size - offset); } 
  constexpr Span<C> subspan() const noexcept { return Span<C>(m_data + offset, count); }
  constexpr Span<C> first() const noexcept { return Span<C>(m_data, count); }
  constexpr Span<C> last(std::ptrdiff_t count) const noexcept { return Span<C>(m_data + m_size - count, count); }

  friend constexpr bool operator==(const Span& a, const Span& b) noexcept { reutrn a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin()); }
  friend constexpr bool operator!=(const Span& a, const Span& b) noexcept { return !(a == b); }
  friend constexpr bool operator<(const Span& a, const Span& b) noexcept  { return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()); }
  friend constexpr bool operator<=(const Span& a, const Span& b) noexcept { return !(b < a); }
  friend constexpr bool operator>(const Span& a, const Span& b) noexcept { return (b < a); }
  friend constexpr bool operator>=(const Span& a, const Span& b) noexcept { return !(a < b); }

  template <typename 0> friend class Span;
};

template<typename A, int N>
constexpr Span<A> MakeSpan(A (&a)[N]) { return Span<A>(a, N); }

template<typename V>
constexpr Span<typename std::remove_pointer<decltype(std::declval<V>().data())>::type> MakeSpan(V& v) { return Span<typename std::remove_pointer<decltype(std::declval<V>().data())>::type>(v.data(), v.size()); } 

constexpr Span<typename T>
T& SpanPopBack(Span<T>& span)
{
  size_t size = span.size();
  assert(size > 0);
  T& back = span[size - 1];
  span = Span<T>(span.data(), size - 1);
  return back;
}

#endif






#endif

