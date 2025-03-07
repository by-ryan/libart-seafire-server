#ifndef art__seafire__server__session_hxx_
#define art__seafire__server__session_hxx_

#include <art/seafire/common/diagnostics.hxx>

#include <art/seafire/common/io/stream.hxx>

#include <art/seafire/server/counter.hxx>
#include <art/seafire/server/error-handler.hxx>
#include <art/seafire/server/request-handler.hxx>

#include <art/seafire/protocol/connection.hxx>

#include <asio.hpp>

#include <memory>
#include <system_error>

namespace art::seafire::server
{

  class supervisor_t;
  class transaction_t;

  class session_t
    : public std::enable_shared_from_this<session_t>
  {
  public:
    class info_t;

    struct stats_t
    {
      /// The total number of transactions handled by this session.
      ///
      counter_t transaction_counter;

    };

    virtual
    ~session_t() noexcept;

    supervisor_t&
    owner();

    stats_t
    stats() const;

    void
    start();

    void
    stop();

  public:
    friend
    std::shared_ptr<session_t>
    make_shared_session(common::diagnostics_t&,
                        error_handler_t&,
                        supervisor_t&,
                        std::unique_ptr<common::io::stream_t>,
                        request_handler_t&);

    session_t(common::diagnostics_t&,
              error_handler_t&,
              supervisor_t&,
              std::unique_ptr<common::io::stream_t>,
              request_handler_t&);

    session_t(session_t const&) = delete;
    session_t(session_t&&) = delete;

    void
    init_transaction();

    void
    on_tx_complete(std::error_code const&, bool);

    void
    init_close();

    void
    init_close_read();

    void
    on_close_read(std::error_code const&);

    session_t& operator=(session_t const&) = delete;
    session_t& operator=(session_t&&) = delete;

  private:
    common::diagnostics_t::proxy_t
    trace();

    std::mutex protector_;

    common::diagnostics_t& diagnostics_;
    error_handler_t& error_handler_;
    supervisor_t& owner_;
    std::unique_ptr<common::io::stream_t> stream_;
    request_handler_t& handler_;
    protocol::connection_t connection_;
    stats_t stats_;
    std::shared_ptr<transaction_t> current_tx_;

    char throwaway_[1];

  };

  class session_t::info_t
  {
  };

  inline
  std::shared_ptr<session_t>
  make_shared_session(common::diagnostics_t& diagnostics,
                      error_handler_t& error_handler,
                      supervisor_t& supervisor,
                      std::unique_ptr<common::io::stream_t> stream,
                      request_handler_t& request_handler)
  {
    return std::make_shared<session_t>(diagnostics,
                                       error_handler,
                                       supervisor,
                                       std::move(stream),
                                       request_handler);
  }

} // namespace art::seafire::server

#endif
