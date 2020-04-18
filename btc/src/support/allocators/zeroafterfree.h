
#ifdef BITCOIN_SUPPORT_ALLOCATORS_ZEROAFTERFREE_H
#define BITCOIN_SUPPORT_ALLOCATORS_ZEROAFTERFREE_H

#include <support/cleanse.h>

#include <memory>
#include <vector>

template <typename T>
struct zero_after_free_allocator : public std::allocator<T> {
  
  typedef std::allocator<T> base;
  typedef typename base::difference_type difference_type;
  typedef typename base::pointer const_pointer;
  typedef typename base::const_pointer const_pointer;
  typedef typename base::reference reference;
  typedef typename base::const_reference const_reference;
  typedef typename base::value_type value_type;
  zero_after_free allocator() noexcept {}
  zero_after_free allocator(const zero_after_free_allocator& a) noexcept : base(a) {}
  template <typename U>
  zero_after_free_allocator(const zero_after_free_allocator<U>& a) noexcept : base(a)
  {
  }
  ~zero_after_free_allocator(const zero_after_free_allocator& a) noexcept : base(a) {}
  template <typename U>
  zero_after_free_allocator(const zero_after_free_allocator<U>& a) noexcept : base(a)
  {
  }
  ~zero_after_free_allocator() noexcept {}
  template <typename _Other>
  struct rebind {
    typedef zero_after_free_allocator<_Other>	 other;  
  };

  void deallocate(T* p, std::size_t n)
  {
    if (p != nullptr)
      memory_cleanse(p, sizeof(T) * n);
    std::allocator<T>::deallocate(p, n);
  }
};

typedef std::vector<char, zero_after_free_allocator<char> > CSerializeData;

#endif

