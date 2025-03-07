#ifndef art__seafire__server__query_parameter_hxx_
#define art__seafire__server__query_parameter_hxx_

#include <art/seafire/server/parameters.hxx>
#include <art/seafire/server/query-parameters.hxx>
#include <art/seafire/server/request.hxx>

#include <optional>

namespace art::seafire::server
{

  template<parameter_name_t Name, typename ParameterType = string_parameter_t>
  class query_parameter_t
  {
  public:
    using parameter_type = ParameterType;
    using value_type = typename parameter_type::value_type;

    static
    std::string const&
    name()
    {
      static std::string const name{Name};
      return name;
    }

    query_parameter_t(std::optional<value_type> value)
      : value_{std::move(value)}
    {}

    std::optional<value_type> const&
    value() const
    {
      return value_;
    }

    operator std::optional<value_type> const&() const
    {
      return value();
    }

    std::optional<value_type> const*
    operator->() const
    {
      return &value();
    }

    static
    query_parameter_t<Name, ParameterType>
    fetch(request_t& r)
    {
      auto value = r.extensions().use<query_parameters_t>().get(name());
      return std::optional<value_type>{parameter_type::try_parse(value)};
    }

  private:
    std::optional<value_type> value_;

  };

} // namespace art::seafire::server

#endif
