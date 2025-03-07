#ifndef art__seafire__server__transaction_hxx_
#define art__seafire__server__transaction_hxx_

#include <art/seafire/common/allocator.hxx>
#include <art/seafire/common/diagnostics.hxx>
#include <art/seafire/common/extension-context.hxx>

#include <art/seafire/common/io/buffer.hxx>

#include <art/seafire/server/error-handler.hxx>
#include <art/seafire/server/request-handler.hxx>
#include <art/seafire/server/request.hxx>
#include <art/seafire/server/response.hxx>

#include <art/seafire/protocol/connection.hxx>

#include <asio.hpp>

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>

namespace art::seafire::server
{

  class transaction_t
    : public std::enable_shared_from_this<transaction_t>,
      request_t,
      response_t
  {
  public:
    struct configuration_t;

    /// The result of this transaction.
    ///
    enum result_t
    {
      /// Indicates a completed transation.
      ///
      complete,

      /// Indicates a completed transation and that the
      /// underlying connection should be closed.
      complete_closed

    };

    /// Completion handler type.
    ///
    using completion_handler_t = std::function<void(std::error_code, result_t)>;

    virtual
    ~transaction_t() noexcept(false);

    error_handler_t&
    get_error_handler();

    protocol::connection_t&
    connection();

    asio::any_io_executor const&
    get_executor();

    void
    start();

    void
    cancel();

    // =================
    // Common interface.
    //

    common::allocator_t&
    memory() override;

    // ==================
    // Request interface.
    //
    // fixme: make these private.
    //

    protocol::request_t const&
    get_request() const override;

    std::istream&
    get_request_content() override;

    common::extension_context_t&
    get_request_extensions() override;

    // ===================
    // Response interface.
    //
    // fixme: make these private.
    //

    void
    register_finalizer(finalizer_t* f) override;

    void
    deregister_finalizer(finalizer_t* f) override;

    void
    suppress_finalizers() override;

    protocol::response_t&
    get_response() override;

    protocol::response_t const&
    get_response() const override;

    common::extension_context_t&
    get_response_extensions() override;

    void
    do_send_response(protocol::status_code_t,
                     common::io::const_buffers_t const&) override;

    void
    do_send_error(common_error_t error) override;

  protected:
    template<typename... Args>
    friend
    std::shared_ptr<transaction_t>
    make_transaction(Args&&...);

  protected:
    /// fixme: replace request_timeout with a configuration_t object.
    ///
    transaction_t(common::diagnostics_t&,
                  std::chrono::seconds request_timeout,
                  error_handler_t&,
                  protocol::connection_t&,
                  request_handler_t&,
                  completion_handler_t);

  private:
    common::diagnostics_t::proxy_t
    trace();

    bool
    keep_alive();

    void
    init_read();

    void
    on_read_timeout(std::error_code const&);

    void
    on_read(std::error_code const&);

    void
    prepare_response();

    void
    init_dispatch();

    void
    invoke_finalizers();

    void
    finalize_response(protocol::status_code_t const&,
                      std::size_t);

    void
    init_write(common::io::const_buffers_t const&);

    void
    on_write(std::error_code const&);

    std::recursive_mutex protector_;

    common::diagnostics_t& diagnostics_;
    std::chrono::seconds request_timeout_;
    error_handler_t& error_handler_;
    common::allocator_t allocator_;
    common::extension_context_t request_extensions_;
    common::extension_context_t response_extensions_;
    protocol::connection_t& connection_;
    request_handler_t& handler_;
    completion_handler_t on_completion_;
    asio::steady_timer request_timeout_timer_;
    protocol::request_t request_;
    asio::streambuf request_content_;
    std::istream request_content_stream_;
    protocol::response_t response_;
    std::vector<finalizer_t*> finalizers_;

  };

  /// Holds transaction configuration parameters.
  ///
  struct transaction_t::configuration_t
  {
    /// Holds the request timeout (in seconds).
    ///
    /// Request timeout is completely disabled if this is 0.
    ///
    std::chrono::seconds request_timeout;

  };

  template<typename... Args>
  std::shared_ptr<transaction_t>
  make_transaction(Args&&... args)
  {
    return std::shared_ptr<transaction_t>{
      new transaction_t{
        std::forward<Args>(args)...
      }
    };
  }

} // namespace art::seafire::server

#endif
