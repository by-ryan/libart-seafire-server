#include <art/seafire/server/session.hxx>

#include <art/seafire/server/diagnostics.hxx>
#include <art/seafire/server/supervisor.hxx>
#include <art/seafire/server/transaction.hxx>

#include <art/seafire/common/io/error.hxx>

#include <asio.hpp>

namespace art::seafire::server
{

  session_t::
  ~session_t() noexcept
  {
    trace() << "~session_t()...";
  }

  supervisor_t&
  session_t::
  owner()
  {
    return owner_;
  }

  session_t::stats_t
  session_t::
  stats() const
  {
    return stats_;
  }

  void
  session_t::
  start()
  {
    trace() << "start()...";
    init_transaction();
  }

  void
  session_t::
  stop()
  {
    // fixme: should we really ignore the error code at this point?
    //

    std::error_code ignored_ec;
    stream_->close(ignored_ec);
  }

  session_t::
  session_t(common::diagnostics_t& diagnostics,
            error_handler_t& error_handler,
            supervisor_t& owner,
            std::unique_ptr<common::io::stream_t> stream,
            request_handler_t& handler)
    : diagnostics_{diagnostics},
      error_handler_{error_handler},
      owner_{owner},
      stream_{std::move(stream)},
      handler_{handler},
      connection_{*stream_, 1024*1024} // FIXME: magic number.
  {
    trace() << "session()...";
  }

  void
  session_t::
  init_transaction()
  {
    trace() << "init_transaction()";

    auto self = shared_from_this();

    auto bound = [this, self](std::error_code const& ec,
                              transaction_t::result_t result)
    {
      on_tx_complete(ec, result == transaction_t::result_t::complete_closed);
    };

    // fixme: This will potentially destroy the previous transaction.
    //        Should we detect this and throw? Shouldn't ever happen...
    //
    current_tx_ = make_transaction(diagnostics_,
                                   std::chrono::seconds{0},
                                   error_handler_,
                                   connection_,
                                   handler_,
                                   bound);

    current_tx_->start();
  }

  void
  session_t::
  on_tx_complete(std::error_code const& ec, bool close)
  {
    trace() << "on_tx_complete()...";

    auto self = shared_from_this();

    current_tx_.reset();

    if (!ec) {
      if (close) {
        trace() << "on_tx_complete(): close requested...";

        // Initiate a graceful close of this session/connection.
        //
        init_close();
      }
      else {
        trace() << "on_tx_complete(): keep-alive requested...";

        // We're apparently not done, initiate a new transaction.
        //
        init_transaction();
      }
    }
    else {
      trace() << "on_tx_complete(): error: " << ec;

      // If an error occurred, we just remove ourself from the
      // supervisor which will eventually close the connection
      // when we are destroyed.
      //
      owner().remove(self);
    }
  }

  void
  session_t::
  init_close()
  {
    trace() << "graceful close initiated...";

    auto self = shared_from_this();

    auto bound = [this, self]()
    {
      trace() << "graceful close complete!";
      owner().remove(self);
    };

    stream_->async_graceful_close(bound);
  }

  common::diagnostics_t::proxy_t
  session_t::
  trace()
  {
    return diagnostics_ << session_category();
  }

} // namespace art::seafire::server
