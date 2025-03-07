#include <art/seafire/server/request.hxx>

#include <art/seafire/protocol/rfc7230/connection.hxx>

namespace art::seafire::server
{

  protocol::request_t const&
  request_t::
  get_message() const
  {
    return get_request();
  }

  std::istream&
  request_t::
  content()
  {
    return get_request_content();
  }

  common::extension_context_t&
  request_t::
  extensions()
  {
    return get_request_extensions();
  }

  request_t::
  request_t() = default;

  request_t::
  ~request_t() noexcept = default;

} // namespace art::seafire::server
