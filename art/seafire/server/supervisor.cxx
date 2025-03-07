#include <art/seafire/server/supervisor.hxx>

#include <art/seafire/server/diagnostics.hxx>

namespace art::seafire::server
{

  supervisor_t::
  supervisor_t(common::diagnostics_t& diagnostics,
               error_handler_t& error_handler,
               request_handler_t& handler)
    : diagnostics_{diagnostics},
      error_handler_{error_handler},
      handler_{handler}
  {}

  void
  supervisor_t::
  async_accept(common::io::acceptor_t& acceptor, accept_handler_t handler)
  {
    trace() << "async_accept()...";

    auto bound = [this, handler](std::error_code const& ec,
                                 std::unique_ptr<common::io::stream_t> stream)
    {
      handler(ec);

      if (!ec) {
        this->start(std::move(stream));
      }
    };

    acceptor.async_accept(bound);
  }

  void
  supervisor_t::
  start(std::unique_ptr<common::io::stream_t> stream)
  {
    trace() << "start()...";

    if (!stream) {
      throw std::invalid_argument{"stream"};
    }

    auto session = make_shared_session(diagnostics_,
                                       error_handler_,
                                       *this,
                                       std::move(stream),
                                       handler_);

    {
      std::lock_guard lock{protector_};
      sessions_.emplace(session);
    }

    session->start();
  }

  void
  supervisor_t::
  stop_all()
  {
    trace() << "stop_all()...";

    std::lock_guard lock{protector_};

    for (auto const& j : sessions_) {
      j->stop();
    }
  }

  common::diagnostics_t::proxy_t
  supervisor_t::
  trace() const
  {
    return diagnostics_ << supervisor_category();
  }

  void
  supervisor_t::
  remove(session_ptr_t session)
  {
    std::lock_guard lock{protector_};
    sessions_.erase(session);
  }

} // namespace art::seafire::server
