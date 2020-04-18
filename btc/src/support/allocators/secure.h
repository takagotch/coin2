
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
}

