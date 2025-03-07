#ifndef art__seafire__server__request_hxx_
#define art__seafire__server__request_hxx_

#include <art/seafire/protocol/request.hxx>

#include <art/seafire/common/allocator.hxx>
#include <art/seafire/common/extension-context.hxx>

#include <art/seafire/common/io/buffer.hxx>

#include <asio.hpp>

namespace art::seafire::server
{

  /// Represents a server-side request.
  ///
  class request_t
  {
  public:
    protocol::request_t const&
    get_message() const;

    std::istream&
    content();

    // fixme: replace with non-virtual function and virtual
    // get_request_allocator function.
    //
    virtual
    common::allocator_t&
    memory() = 0;

    common::extension_context_t&
    extensions();

  protected:
    request_t();

    request_t(request_t const&) = delete;
    request_t(request_t&&) = delete;

    ~request_t() noexcept;

    request_t& operator=(request_t const&) = delete;
    request_t& operator=(request_t&&) = delete;

  private:
    virtual
    protocol::request_t const&
    get_request() const = 0;

    virtual
    std::istream&
    get_request_content() = 0;

    virtual
    common::extension_context_t&
    get_request_extensions() = 0;

  };

  template<typename Header>
  bool
  has(request_t const&);

  template<typename Header>
  bool
  has_quick(request_t const&);

  template<typename Header>
  auto
  get(request_t const&);

  template<typename Header>
  auto
  get(request_t const&, std::error_code&);

} // namespace art::seafire::server

#include <art/seafire/server/request.txx>

#endif
