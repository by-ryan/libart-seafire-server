#ifndef art__seafire__server__supervisor_hxx_
#define art__seafire__server__supervisor_hxx_

#include <art/seafire/common/diagnostics.hxx>

#include <art/seafire/common/io/acceptor.hxx>
#include <art/seafire/common/io/stream.hxx>

#include <art/seafire/server/counter.hxx>
#include <art/seafire/server/error-handler.hxx>
#include <art/seafire/server/request-handler.hxx>
#include <art/seafire/server/session.hxx>

#include <functional>
#include <memory>
#include <mutex>
#include <set>

namespace art::seafire::server
{

  /// Implements a session supervisor.
  ///
  class supervisor_t
  {
    friend session_t;

    using session_ptr_t = std::shared_ptr<session_t>;

  public:
    class info_t;

    using accept_handler_t = std::function<void(std::error_code)>;

    /// Tracks supervisor statistics counters.
    ///
    struct counters_t
    {
      /// Tracks the total number of sessions.
      ///
      counter_t total_session_count;

      /// Tracks the total number of currently active sessions.
      ///
      counter_t active_session_count;

    };

    supervisor_t(common::diagnostics_t&,
                 error_handler_t&,
                 request_handler_t&);

    supervisor_t(supervisor_t const&) = delete;
    supervisor_t(supervisor_t&&) = delete;

    void
    async_accept(common::io::acceptor_t&, accept_handler_t);

    void
    start(std::unique_ptr<common::io::stream_t>);

    void
    stop_all();

    supervisor_t& operator=(supervisor_t const&) = delete;
    supervisor_t& operator=(supervisor_t&&) = delete;

  private:
    common::diagnostics_t::proxy_t
    trace() const;

    void
    remove(session_ptr_t);

    std::mutex protector_;

    common::diagnostics_t& diagnostics_;
    error_handler_t& error_handler_;
    request_handler_t& handler_;
    std::set<session_ptr_t> sessions_;
    counters_t stats_;

  };

  class supervisor_t::info_t
  {
  public:
  };

} // namespace art::seafire::server

#endif
