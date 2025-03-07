#ifndef art__seafire__server__query_parameters_hxx_
#define art__seafire__server__query_parameters_hxx_

#include <map>
#include <optional>
#include <string>

namespace art::seafire::server
{

  class query_parameters_t
  {
  public:
    query_parameters_t(std::map<std::string, std::string>);

    std::optional<std::string>
    get(std::string const&) const;

    static
    std::optional<query_parameters_t>
    try_parse(std::string const&);

  private:
    std::map<std::string, std::string> keys_;

  };

} // namespace art::seafire::server

#endif
