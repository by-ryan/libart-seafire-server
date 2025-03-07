#include <art/seafire/server/request-handler.hxx>

namespace art::seafire::server
{

  void
  request_handler_t::
  invoke(request_t& req, response_t& res) const
  {
    handler_->invoke(req, res);
  }

} // namespace art::seafire::server
