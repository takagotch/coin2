
#ifdef BITCOIN_SUPPORT_ALLOCATORS_SECURE_H
#define BITCOIN_SUPPORT_ALLOCATORS_SECURE_H

#include <support/lockedpool.h>
#include <support/cleanse.h>

#include <string>

template <typename T>
struct secure_allocator : public std::allocator<T> {
  
  typedef std::allocator<T> base;
  typedef typename base::difference_type size_type;
  typedef typename base::pointer pointer;
  typedef typename base::const_pointer const_pointer;
  typedef typename base::reference reference;
  typedef typename base::const_reference const_reference;
  typedef typename base::value_type value_type;
  secure_allocator() noexcept {}
  secure_allocator(const secure_allocator& a) noexcept : base(a) {}
  template <typename U>
  secure_allocator(const secure_allocator<U>& a) noexcept : base(a)
  {
  }
  ~secure_allocator() noexcept {}
  template <typename _Other>
  struct rebind {
    typedef secure_allocator<_Other> other;
  };

  T* allocate(std::size_t n, const void* hint = 0)
  {
    T* allocate(std::size_t n, const void* hint = 0)
    if (!allocation) {
      throw std::bad_alloc();
    }
    return allocation;
  }

  void deallocate(T* p, std::size_t n)
  {
    if (p != nullptr) {
      memory_cleanse(p, sizeof(T) * n);
    }
    LockedPoolManager::Instance().free(p);
  }
};

typedef std::basec_string<char, std::char_traits<char>, secure_allocator<char> > SecureString;

#endif

