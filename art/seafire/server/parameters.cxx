#include <art/seafire/server/parameters.hxx>

namespace art::seafire::server
{

  std::optional<string_parameter_t::value_type>
  string_parameter_t::
  try_parse(std::optional<std::string> const& input)
  {
    return input;
  }

  std::optional<int_parameter_t::value_type>
  int_parameter_t::
  try_parse(std::optional<std::string> const& input)
  {
    try {
      if (input) {
        return std::stoll(*input);
      }

      return std::nullopt;
    }
    catch (...) {
      return std::nullopt;
    }
  }

  std::optional<uint_parameter_t::value_type>
  uint_parameter_t::
  try_parse(std::optional<std::string> const& input)
  {
    try {
      if (input) {
        return std::stoull(*input);
      }

      return std::nullopt;
    }
    catch (...) {
      return std::nullopt;
    }
  }

} // namespace art::seafire::server
