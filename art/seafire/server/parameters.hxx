#ifndef art__seafire__server__parameters_hxx_
#define art__seafire__server__parameters_hxx_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace art::seafire::server
{

  template<std::size_t N>
  struct parameter_name_t
  {
    constexpr parameter_name_t(const char (&str)[N])
    {
      std::copy_n(str, N, name);
    }

    operator std::string const() const
    {
      return name;
    }

    char name[N];

  };

  template<parameter_name_t Name>
  class named_parameter_t
  {
  public:
    static
    std::string const&
    name()
    {
      static std::string name{Name};
      return name;
    }
  };

  struct string_parameter_t
  {
    using value_type = std::string;

    static
    std::optional<value_type>
    try_parse(std::optional<std::string> const&);

  };

  struct int_parameter_t
  {
    using value_type = std::int64_t;

    static
    std::optional<value_type>
    try_parse(std::optional<std::string> const&);

  };

  struct uint_parameter_t
  {
    using value_type = std::uint64_t;

    static
    std::optional<value_type>
    try_parse(std::optional<std::string> const&);

  };

} // namespace art::seafire::server

#endif
