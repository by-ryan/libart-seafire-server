#include <art/seafire/server/query-parameters.hxx>

namespace art::seafire::server
{

  query_parameters_t::
  query_parameters_t(std::map<std::string, std::string> keys)
    : keys_{std::move(keys)}
  {}

  std::optional<std::string>
  query_parameters_t::
  get(std::string const& key) const
  {
    if (auto it = keys_.find(key); it != keys_.end())
      return it->second;

    return std::nullopt;
  }

  std::optional<query_parameters_t>
  query_parameters_t::
  try_parse(std::string const& q)
  {
    std::map<std::string, std::string> keys;

    for (auto it = q.begin(); it != q.end();) {
      auto k = it;

      while (it != q.end() && *it != '=' && *it != '&') {
        ++it;
      }

      auto kend = it;

      if (it != q.end() && *it == '=') {
        ++it; // skips '='
      }

      auto v = it;

      while (it != q.end() && *it != '&') {
        ++it;
      }

      auto vend = it;

      if (it != q.end() && *it == '&') {
        ++it; // skips '&'
      }

      std::string key{k, kend};
      std::string value{v, vend};

      keys.emplace(std::move(key), std::move(value));
    }

    return keys;
  }

} // namespace art::seafire::server
