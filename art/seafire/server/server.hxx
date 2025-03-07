#ifndef art__seafire__server__hxx_
#define art__seafire__server__hxx_

#include <art/seafire/server/common-error.hxx>
#include <art/seafire/server/configuration.hxx>
#include <art/seafire/server/counter.hxx>
#include <art/seafire/server/request-handler.hxx>
#include <art/seafire/server/supervisor.hxx>

#include <art/seafire/common/diagnostics.hxx>

#include <art/seafire/common/io/acceptor.hxx>

namespace art::seafire::server
{

  class request_t;
  class response_t;
  class transaction_t;

  class server_t
    : public error_handler_t
  {
  public:
    /// Acceptor set type.
    ///
    using acceptor_set_t = std::set<std::unique_ptr<common::io::acceptor_t>>;

    server_t(common::diagnostics_t&,
             configuration_t,
             acceptor_set_t,
             request_handler_t);

    server_t(server_t const&) = delete;
    server_t(server_t&&) = delete;

    void
    start();

    void
    stop(bool = false);

    server_t& operator=(server_t const&) = delete;
    server_t& operator=(server_t&&) = delete;

  protected:
    void
    on_error(request_t&, response_t&, common_error_t) override;

    void
    on_exception(request_t&, response_t&) noexcept override;

    // fixme: void shutdown_stream();

  private:
    common::diagnostics_t::proxy_t
    trace();

    void
    init_accept(common::io::acceptor_t&);

    void
    on_accept(common::io::acceptor_t&, std::error_code const&);

    common::diagnostics_t& diagnostics_;
    acceptor_set_t const acceptors_;
    request_handler_t handler_;
    supervisor_t supervisor_;

  };

} // namespace art::seafire::server

#endif
