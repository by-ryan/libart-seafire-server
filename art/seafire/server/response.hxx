#ifndef art__seafire__server__response_hxx_
#define art__seafire__server__response_hxx_

#include <art/seafire/server/common-error.hxx>

#include <art/seafire/protocol/response.hxx>
#include <art/seafire/protocol/status-code.hxx>

#include <art/seafire/common/allocator.hxx>
#include <art/seafire/common/extension-context.hxx>

#include <asio.hpp>

#include <functional>
#include <iostream>

namespace art::seafire::server
{

  class request_t;

  /// Represents a server-side response.
  ///
  class response_t
  {
  public:
    class stream_t;
    class finalizer_t;

    friend finalizer_t;

    protocol::response_t&
    get_message();

    protocol::response_t const&
    get_message() const;

    virtual
    void
    suppress_finalizers() = 0;

    void
    send(protocol::status_code_t);

    void
    send(protocol::status_code_t, common::io::const_buffer_t const&);

    void
    send(protocol::status_code_t, common::io::const_buffers_t const&);

    void
    send(protocol::status_code_t, stream_t const&);

    void
    send(common_error_t);

    stream_t
    allocate_stream();

    // fixme: replace with non-virtual function and virtual
    // get_response_allocator function.
    //
    virtual
    common::allocator_t&
    memory() = 0;

    common::extension_context_t&
    extensions();

  protected:
    response_t();

    response_t(response_t const&) = delete;
    response_t(response_t&&) = delete;

    ~response_t() noexcept;

    virtual
    void
    register_finalizer(finalizer_t*) = 0;

    virtual
    void
    deregister_finalizer(finalizer_t*) = 0;

    void
    invoke_finalizer(finalizer_t*, request_t&);

    response_t& operator=(response_t const&) = delete;
    response_t& operator=(response_t&&) = delete;

  private:
    virtual
    protocol::response_t&
    get_response() = 0;

    virtual
    protocol::response_t const&
    get_response() const = 0;

    virtual
    common::extension_context_t&
    get_response_extensions() = 0;

    virtual
    void
    do_send_response(protocol::status_code_t,
                     common::io::const_buffers_t const&) = 0;

    virtual
    void
    do_send_error(common_error_t) = 0;

  };

  template<typename Header>
  bool
  has(response_t const& r)
  {
    return protocol::has<Header>(r.get_message());
  }

  template<typename Header>
  bool
  has_quick(response_t const& r)
  {
    return protocol::has_quick<Header>(r.get_message());
  }

  template<typename Header>
  auto
  get(response_t const& r)
  {
    return protocol::get<Header>(r.get_message());
  }

  template<typename Header>
  auto
  get(response_t const& r, std::error_code& ec)
  {
    return protocol::get<Header>(r.get_message(), ec);
  }

  template<typename Header, typename... Args>
  void
  set(response_t& r, Args&&... args)
  {
    protocol::set<Header>(r.get_message(), std::forward<Args>(args)...);
  }

  template<typename Header>
  void
  erase(response_t& r)
  {
    protocol::erase<Header>(r.get_message());
  }

  template<typename Header, typename... Args>
  void
  set_if_not_set(response_t& r, Args&&... args)
  {
    protocol::set_if_not_set<Header>(r.get_message(), std::forward<Args>(args)...);
  }

  class response_t::stream_t
    : public std::iostream
  {
  public:
    asio::streambuf*
    rdbuf() const;

    std::size_t
    size() const;

  private:
    friend response_t;

    explicit
    stream_t(asio::streambuf*);

    asio::streambuf* rdbuf_;

  };

  class response_t::finalizer_t
  {
  public:
    using function_t = std::function<void(request_t&, response_t&)>;

    finalizer_t(response_t&, function_t);

    finalizer_t(finalizer_t const&) = delete;
    finalizer_t(finalizer_t&&) = delete;

    ~finalizer_t() noexcept;

    finalizer_t& operator=(finalizer_t const&) = delete;
    finalizer_t& operator=(finalizer_t&&) = delete;

  private:
    friend response_t;

    void
    invoke(request_t&, response_t&);

    response_t& response_;
    function_t f_;

  };

} // namespace art::seafire::server

#endif
