#include "art/seafire/protocol/status-code.hxx"
#include <art/seafire/server/transaction.hxx>

#include <art/seafire/server/diagnostics.hxx>
#include <art/seafire/server/version.hxx>

#include <art/seafire/protocol/rfc7230/connection.hxx>
#include <art/seafire/protocol/rfc7230/content-length.hxx>
#include <art/seafire/protocol/rfc7231/content-type.hxx>
#include <art/seafire/protocol/rfc7231/date.hxx>
#include <art/seafire/protocol/rfc7231/server.hxx>

namespace art::seafire::server
{

  // fixme: determine which member functions need to lock the mutex.
  //

  transaction_t::
  ~transaction_t() noexcept(false)
  {
    trace() << "~transaction_t()...";
  }

  error_handler_t&
  transaction_t::
  get_error_handler()
  {
    return error_handler_;
  }

  protocol::connection_t&
  transaction_t::
  connection()
  {
    return connection_;
  }

  asio::any_io_executor const&
  transaction_t::
  get_executor()
  {
    return connection().get_executor();
  }

  void
  transaction_t::
  start()
  {
    init_read();
  }

  void
  transaction_t::
  cancel()
  {
    // fixme: connection().cancel();
  }

  common::allocator_t&
  transaction_t::
  memory()
  {
    return allocator_;
  }

  protocol::request_t const&
  transaction_t::
  get_request() const
  {
    return request_;
  }

  std::istream&
  transaction_t::
  get_request_content()
  {
    return request_content_stream_;
  }

  common::extension_context_t&
  transaction_t::
  get_request_extensions()
  {
    return request_extensions_;
  }

  void
  transaction_t::
  register_finalizer(finalizer_t* f)
  {
    if (f == nullptr) {
      throw std::invalid_argument{"invalid finalizer"};
    }

    std::lock_guard lock{protector_};
    finalizers_.emplace_back(f);
  }

  void
  transaction_t::
  deregister_finalizer(finalizer_t* f)
  {
    if (f == nullptr) {
      throw std::invalid_argument{"invalid finalizer"};
    }

    std::lock_guard lock{protector_};
    std::erase(finalizers_, f);
  }

  void
  transaction_t::
  suppress_finalizers()
  {
    std::lock_guard lock{protector_};
    finalizers_.clear();
  }

  protocol::response_t&
  transaction_t::
  get_response()
  {
    return response_;
  }

  protocol::response_t const&
  transaction_t::
  get_response() const
  {
    return response_;
  }

  common::extension_context_t&
  transaction_t::
  get_response_extensions()
  {
    return response_extensions_;
  }

  void
  transaction_t::
  do_send_response(protocol::status_code_t s,
                   common::io::const_buffers_t const& content)
  {
    trace() << "do_send_response()...";

    std::lock_guard lock{protector_};

    auto self = shared_from_this();

    invoke_finalizers();
    finalize_response(s, asio::buffer_size(content));
    init_write(content);
  }

  void
  transaction_t::
  do_send_error(common_error_t error)
  {
    auto self = shared_from_this();

    auto bound = [this, self, error]
    {
      error_handler_.on_error(*this, *this, error);
    };

    asio::post(get_executor(), bound);
  }

  transaction_t::
  transaction_t(common::diagnostics_t& diagnostics,
                std::chrono::seconds request_timeout,
                error_handler_t& error_handler,
                protocol::connection_t& connection,
                request_handler_t& handler,
                completion_handler_t on_completion)
    : diagnostics_{diagnostics},
      request_timeout_{request_timeout},
      error_handler_{error_handler},
      connection_{connection},
      handler_{handler},
      on_completion_{on_completion},
      request_timeout_timer_{connection_.get_executor()},
      request_content_{1024 * 1024 * 32}, // fixme: make configurable buffer max,
      request_content_stream_{&request_content_}
  {
    trace() << "transaction_t()...";
  }

  common::diagnostics_t::proxy_t
  transaction_t::
  trace()
  {
    return diagnostics_ << transaction_category();
  }

  bool
  transaction_t::
  keep_alive()
  {
    auto c = get<protocol::rfc7230::connection_t>(get_request());

    if (c && c->close()) {
      trace() << "close requested by request!";
      return false;
    }

    if (get_request().version() == protocol::http_1_0 && c) {
      trace() << "close requested by protocol version";
      return c->keep_alive();
    }

    if (get_request().version() == protocol::http_1_1) {
      trace() << "keep-live requested by protocol version";
      return true;
    }

    trace() << "defaulting to close";

    return false;
  }

  void
  transaction_t::
  init_read()
  {
    trace() << "init_read()...";

    auto self = shared_from_this();
    prepare_response();

    if (request_timeout_ > std::chrono::seconds{0}) {
      auto on_timeout = [this, self](std::error_code const& ec)
      {
        on_read_timeout(ec);
      };

      request_timeout_timer_.expires_after(request_timeout_);
      request_timeout_timer_.async_wait(on_timeout);
    }

    auto on_read = [this, self](std::error_code const& ec)
    {
      this->on_read(ec);
    };

    connection().async_read(request_, request_content_, on_read);
  }

  void
  transaction_t::
  on_read_timeout(std::error_code const& ec)
  {
    auto self = shared_from_this();

    std::lock_guard lock{protector_};

    if (!ec) {
      // fixme: add diagnostics...
      //
      connection_.cancel();
      send(common_error_t::request_timeout);
    }
    else if (ec != asio::error::operation_aborted) {
      // error occurred, restart timer.
      //
      request_timeout_timer_.expires_after(request_timeout_);

      auto on_timeout = [this, self](std::error_code const& ec)
      {
        on_read_timeout(ec);
      };

      request_timeout_timer_.async_wait(on_timeout);
    }
  }

  void
  transaction_t::
  on_read(std::error_code const& ec)
  {
    // Attempt to cancel the request timeout handler, if active.
    //
    if (request_timeout_ > std::chrono::seconds{0}) {
      if (request_timeout_timer_.cancel() < 1) {
        // Timeout already happened.
        //
        return;
      }
    }

    // fixme: add tracing.
    //

    if (ec) {
      // fixme: Send error response based on ec.
      //
      send(common_error_t::internal_server_error);
      return;
    }

    if (auto expect = request_.headers().get_one("expect"); expect) {
      if (request_.version() == protocol::http_1_1 && *expect == "100-continue") {
        static std::string const response{
          "HTTP/1.1 100 Continue\r\n\r\n"
        };

        connection().get_stream().write(common::io::buffer(response));
      }
    }

    init_dispatch();
  }

  void
  transaction_t::
  prepare_response()
  {
    using protocol::rfc7231::date_t;
    using protocol::rfc7231::product_t;
    using protocol::rfc7231::products_t;
    using protocol::rfc7231::server_t;

    using protocol::set;

    set<server_t>(get_response(), products_t{
      product_t{"Seafire", LIBART_SEAFIRE_SERVER_VERSION_STR}
    });
    set<date_t>(get_response(), std::chrono::system_clock::now());

    // we always respond with HTTP/1.1 since that is the highest version we support.
    //
    get_response().set_version(protocol::http_1_1);
  }

  void
  transaction_t::
  init_dispatch()
  {
    trace() << "init_dispatch()...";

    try {
      handler_.invoke(*this, *this);
    }
    catch (...) {
      trace() << "handler threw exception, dispatching to exception handler...";

      try {
        get_error_handler().on_exception(*this, *this);
      }
      catch (...) {
        trace() << "exception handler threw, we're out of luck...";

        // Fuck, we're out of luck.
        //
        send(common_error_t::internal_server_error);
      }
    }
  }

  void
  transaction_t::
  invoke_finalizers()
  {
    trace() << "invoking finalizers...";

    for (auto it = finalizers_.rbegin(); it != finalizers_.rend(); ++it) {
      invoke_finalizer(*it, *this);
    }
  }

  void
  transaction_t::
  finalize_response(protocol::status_code_t const& s,
                    std::size_t content_length)
  {
    trace() << "finalizing response...";

    get_response().set_status(s);

    namespace rfc7230 = protocol::rfc7230;
    namespace rfc7231 = protocol::rfc7231;

    if (get_request().version() == protocol::http_1_0) {
      if (keep_alive()) {
        set<rfc7230::connection_t>(get_response(), "keep-alive");
      }
      else {
        set<rfc7230::connection_t>(get_response(), "close");
      }
    }
    else if (get_request().version() == protocol::http_1_1) {
      if (keep_alive()) {
        erase<rfc7230::connection_t>(get_response());
      }
      else {
        set<rfc7230::connection_t>(get_response(), "close");
      }
    }

    // Make sure content-type is always set.
    //
    if (!has<rfc7231::content_type_t>(get_response())) {
      set<rfc7231::content_type_t>(get_response(), protocol::media_type_t{"application", "octet-stream"});
    }

    // Always set content length to the actual content length.
    //
    set<rfc7230::content_length_t>(get_response(), content_length);
  }

  void
  transaction_t::
  init_write(common::io::const_buffers_t const& content)
  {
    trace() << "init write...()";

    auto self = shared_from_this();
    std::lock_guard lock{protector_};

    auto bound = [this, self](std::error_code const& ec)
    {
      on_write(ec);
    };

    connection().async_write(get_response(), content, bound);
  }

  void
  transaction_t::
  on_write(std::error_code const& ec)
  {
    trace() << "on_write()...";

    auto self = shared_from_this();
    std::lock_guard lock{protector_};

    auto bound = [ec, close = !keep_alive(), cb = on_completion_]()
    {
      cb(ec, close ? complete_closed : complete);
    };

    asio::post(get_executor(), bound);
  }

} // namespace art::seafire::server
