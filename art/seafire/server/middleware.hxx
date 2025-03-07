#ifndef art__seafire__server__middleware_hxx_
#define art__seafire__server__middleware_hxx_

#include <art/seafire/server/request-handler.hxx>
#include <art/seafire/server/request.hxx>
#include <art/seafire/server/response.hxx>

#include <functional>
#include <memory>
#include <vector>

namespace art::seafire::server
{

  template<typename, typename = std::void_t<>>
  struct has_fetch_t : std::false_type {};

  template<typename T>
  struct has_fetch_t<
    T,
    std::void_t<
      decltype(&T::fetch)
    >
  > : std::true_type {};

  template<typename T>
  inline constexpr bool has_fetch_v{has_fetch_t<T>::value};

  template<typename... Args>
  struct middleware_link_traits_t
  {
    using function_type = std::function<
      void(
        request_t&,
        response_t&,
        request_handler_t const&,
        Args...
      )
    >;

    middleware_link_traits_t(function_type const&)
    {}

    template<typename T>
    static
    void
    invoke(T const& handler,
           request_t& req,
           response_t& res,
           request_handler_t const& next)
    {
      if constexpr (has_fetch_v<T>) {
        handler(req, res, next, std::decay_t<Args>::fetch(req)...);
        return;
      }

      handler(req, res, next, req.extensions().use<std::decay_t<Args>>()...);
    }

  };

  /// Implements middleware functionality.
  ///
  class middleware_t
  {
  public:
    template<typename T>
    middleware_t(T handler)
      : handler_{std::make_shared<container_t<T>>(std::move(handler))}
    {}

    void
    invoke(request_t&, response_t&, request_handler_t const&) const;

    template<typename, typename... Args>
    friend
    middleware_t
    make_middleware(Args&&...);

  private:
    struct concept_t
    {
      virtual
      ~concept_t() noexcept = default;

      virtual
      void
      invoke(request_t&, response_t&, request_handler_t const&) const = 0;

    };

    template<typename T>
    struct container_t
      : concept_t
    {
      using traits = decltype(
        middleware_link_traits_t{std::function{std::declval<T>()}}
      );

      template<typename... Args>
      container_t(Args&&... args)
        : handler{std::forward<Args>(args)...}
      {}

      void
      invoke(request_t& req,
             response_t& res,
             request_handler_t const& next) const override
      {
        traits::invoke(handler, req, res, next);
      }

      T handler;

    };

    explicit
    middleware_t(std::shared_ptr<concept_t const>);

    std::shared_ptr<concept_t const> handler_;

  };

#if 0
  template<typename T, typename... Args>
  middleware_t
  make_middleware(Args&&... args)
  {
    return middleware_t{
      std::make_shared<middleware_t::container_t<T>>(std::forward<Args>(args)...)
    };
  }
#endif

  request_handler_t
  make_middleware(std::vector<middleware_t> const&, request_handler_t);

} // namespace art::seafire::server

#endif
