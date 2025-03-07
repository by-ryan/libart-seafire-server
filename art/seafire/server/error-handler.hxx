#ifndef art__seafire__server__error_handler_hxx_
#define art__seafire__server__error_handler_hxx_

#include <art/seafire/server/common-error.hxx>

namespace art::seafire::server
{

  class request_t;
  class response_t;

  class error_handler_t
  {
  public:
    virtual
    void
    on_error(request_t&, response_t&, common_error_t) = 0;

    virtual
    void
    on_exception(request_t&, response_t&) noexcept = 0;

  protected:
    error_handler_t();

    error_handler_t(error_handler_t const&) = delete;
    error_handler_t(error_handler_t&&) = delete;

    ~error_handler_t() noexcept;

    error_handler_t& operator=(error_handler_t const&) = delete;
    error_handler_t& operator=(error_handler_t&&) = delete;

  };

}

#endif
