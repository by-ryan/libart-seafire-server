#ifndef art__seafire__server__counter_hxx_
#define art__seafire__server__counter_hxx_

#include <atomic>
#include <cstdint>

namespace art::seafire::server
{

  template<typename T>
  class atomic_t
    : public std::atomic<T>
  {
  public:
    atomic_t()
      : std::atomic<T>{0}
    {}

    atomic_t(atomic_t<T> const& other)
      : std::atomic<T>{other.load()}
    {}

    atomic_t(atomic_t<T>&& other)
      : std::atomic<T>{other.load()}
    {}

  };

  using counter_t = atomic_t<std::uint64_t>;

} // namespace art::seafire::server

#endif
