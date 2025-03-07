#include <art/seafire/server/middleware.hxx>

namespace art::seafire::server
{

  void
  middleware_t::
  invoke(request_t& req, response_t& res, request_handler_t const& next) const
  {
    handler_->invoke(req, res, next);
  }

  middleware_t::
  middleware_t(std::shared_ptr<concept_t const> handler)
    : handler_{handler}
  {}

  request_handler_t
  make_middleware(std::vector<middleware_t> const& chain,
                  request_handler_t handler)
  {
    struct handler_t
    {
      middleware_t middleware;
      request_handler_t next;

      void
      operator()(request_t& req, response_t& res) const
      {
        middleware.invoke(req, res, next);
      }

    };

    request_handler_t next{std::move(handler)};

    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
      next = handler_t{*it, next};
    }

    return next;
  }

} // namespace art::seafire::server
