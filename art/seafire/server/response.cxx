#include <art/seafire/server/response.hxx>

namespace art::seafire::server
{

  /// Access the response message.
  ///
  protocol::response_t&
  response_t::
  get_message()
  {
    return get_response();
  }

  /// Access the response message.
  ///
  protocol::response_t const&
  response_t::
  get_message() const
  {
    return get_response();
  }

  /// Send response.
  ///
  void
  response_t::
  send(protocol::status_code_t s)
  {
    send(s, common::io::const_buffers_t{});
  }

  /// Send response.
  ///
  void
  response_t::
  send(protocol::status_code_t s, common::io::const_buffer_t const& content)
  {
    send(s, common::io::const_buffers_t{content});
  }

  /// Send response.
  ///
  void
  response_t::
  send(protocol::status_code_t s, common::io::const_buffers_t const& content)
  {
    do_send_response(s, content);
  }

  /// Send response.
  ///
  void
  response_t::
  send(protocol::status_code_t s, stream_t const& content)
  {
    send(s, content.rdbuf()->data());
  }

  /// Send error.
  ///
  void
  response_t::
  send(common_error_t error)
  {
    do_send_error(error);
  }

  /// Allocate a new stream.
  ///
  response_t::stream_t
  response_t::
  allocate_stream()
  {
    return stream_t{&memory().alloc_emplace<asio::streambuf>()};
  }

  /// Access response extensions.
  ///
  common::extension_context_t&
  response_t::
  extensions()
  {
    return get_response_extensions();
  }

  void
  response_t::
  invoke_finalizer(finalizer_t* f, request_t& r)
  {
    if (f) {
      f->invoke(r, *this);
    }
  }

  response_t::
  response_t() = default;

  response_t::
  ~response_t() noexcept = default;

  asio::streambuf*
  response_t::stream_t::
  rdbuf() const
  {
    return rdbuf_;
  }

  std::size_t
  response_t::stream_t::
  size() const
  {
    return rdbuf_->data().size();
  }

  response_t::stream_t::
  stream_t(asio::streambuf* rdbuf)
    : std::iostream{rdbuf ? rdbuf : throw std::invalid_argument{"buf"}},
      rdbuf_{rdbuf}
  {}

  /// Construct a new finalizer.
  ///
  response_t::finalizer_t::
  finalizer_t(response_t& r, function_t f)
    : response_{r}, f_{f}
  {
    response_.register_finalizer(this);
  }

  /// Destroy this finalizer.
  ///
  response_t::finalizer_t::
  ~finalizer_t() noexcept
  {
    response_.deregister_finalizer(this);
  }

  void
  response_t::finalizer_t::
  invoke(request_t& req, response_t& res)
  {
    f_(req, res);
  }

} // namespace art::seafire::server
