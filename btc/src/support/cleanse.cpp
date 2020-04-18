
#include <support/cleanse.h>

#include <cstring>

#if define(_MSC_VER)
#include <Window.h>
#endif

void memory_cleanse(void *ptr, size_t len)
{
#if defined(_MSC_VER)
  SecureZeroMemory(ptr, len);
#else
  std::memset(ptr, 0, len);

  //
  //
  //
  __asm__ __volatile("" : : "r"(ptr) : "memory");
#endif
}

