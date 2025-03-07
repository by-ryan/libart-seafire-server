#ifndef art__seafire__server__request_handler_hxx_
#define art__seafire__server__request_handler_hxx_

#include <memory>

namespace art::seafire::server
{

  class request_t;
  class response_t;

  class request_handler_t
  {
  public:
    template<typename Handler>
    request_handler_t(Handler handler)
      : handler_{
          std::make_shared<container_t<Handler>>(
            std::move(handler)
          )
        }
    {}

    void
    invoke(request_t& req, response_t& res) const;

    template<typename Handler, typename... Args>
    friend
    request_handler_t
    make_request_handler(Args&&...);

  private:
    struct concept_t
    {
      virtual
      ~concept_t() noexcept = default;

      virtual
      void
      invoke(request_t&, response_t&) const = 0;

    };

    template<typename Handler>
    struct container_t
      : concept_t
    {
      template<typename... Args>
      container_t(Args&&... args)
        : handler_{std::forward<Args>(args)...}
      {}

      void
      invoke(request_t& req, response_t& res) const override
      {
        handler_(req, res);
      }

      Handler handler_;
    };

    request_handler_t(std::shared_ptr<concept_t> handler)
      : handler_{std::move(handler)}
    {}

    // Request handlers are shared for performance reasons and hence const.
    //
    std::shared_ptr<concept_t const> handler_;

  };

  template<typename Handler, typename... Args>
  request_handler_t
  make_request_handler(Args&&... args)
  {
    using container_t = request_handler_t::container_t<Handler>;
    return std::make_shared<container_t>(std::forward<Args>(args)...);
  }

} // namespace art::seafire::server

#endif
