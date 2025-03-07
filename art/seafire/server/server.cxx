#include <art/seafire/server/diagnostics.hxx>
#include <art/seafire/server/response.hxx>
#include <art/seafire/server/server.hxx>

#include <art/seafire/protocol/rfc7231/content-type.hxx>

#include <art/seafire/common/io/buffer.hxx>
#include <art/seafire/common/io/error.hxx>

#include <asio.hpp>

namespace art::seafire::server
{

  /// Construct a new HTTP server using the specified acceptors
  /// and request handler.
  ///
  server_t::
  server_t(common::diagnostics_t& diagnostics,
           configuration_t configuration,
           acceptor_set_t acceptors,
           request_handler_t handler)
    : diagnostics_{diagnostics},
      acceptors_{std::move(acceptors)},
      handler_{handler},
      supervisor_{diagnostics_, *this, handler_}
  {}

  void
  server_t::
  start()
  {
    trace() << "starting Seafire server...";

    for (auto const& j : acceptors_) {
      init_accept(*j);
    }
  }

  void
  server_t::
  stop(bool quick)
  {
    trace() << "Seafire server stop requested...";

    if (quick) {
      supervisor_.stop_all();
    }
  }

  void
  server_t::
  on_error(request_t& req, response_t& res, common_error_t error)
  {
    trace() << "on_error()...";

    namespace rfc7231 = protocol::rfc7231;

    switch (error) {
      case common_error_t::bad_request: {
        static std::string const message{"Bad request\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(400, common::io::buffer(message));
        return;
      }

      case common_error_t::payment_required: {
        static std::string const message{"Payment required\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(402, common::io::buffer(message));
        return;
      }

      case common_error_t::forbidden: {
        static std::string const message{"Forbidden\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(403, common::io::buffer(message));
        return;
      }

      case common_error_t::not_found: {
        static std::string const message{"Not found\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(404, common::io::buffer(message));
        return;
      }

      case common_error_t::method_not_allowed: {
        static std::string const message{"Method not allowed\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(405, common::io::buffer(message));
        return;
      }

      case common_error_t::not_acceptable: {
        static std::string const message{"Not acceptable\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(406, common::io::buffer(message));
        return;
      }

      case common_error_t::request_timeout: {
        static std::string const message{"Request timeout\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(408, common::io::buffer(message));
        return;
      }

      case common_error_t::conflict: {
        static std::string const message{"Conflict\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(409, common::io::buffer(message));
        return;
      }

      case common_error_t::gone: {
        static std::string const message{"Gone\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(410, common::io::buffer(message));
        return;
      }

      case common_error_t::length_required: {
        static std::string const message{"Length required\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(411, common::io::buffer(message));
        return;
      }

      case common_error_t::payload_too_large: {
        static std::string const message{"Payload too large\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(413, common::io::buffer(message));
        return;
      }

      case common_error_t::uri_too_long: {
        static std::string const message{"Target URI too long\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(414, common::io::buffer(message));
        return;
      }

      case common_error_t::unsupported_media_type: {
        static std::string const message{"Unsupported media type\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(415, common::io::buffer(message));
        return;
      }

      case common_error_t::expectation_failed: {
        static std::string const message{"Expectation failed\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(417, common::io::buffer(message));
        return;
      }

      case common_error_t::upgrade_required: {
        static std::string const message{"Upgrade required\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(426, common::io::buffer(message));
        return;
      }


      case common_error_t::internal_server_error: {
        static std::string const message{"Internal server error\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(500, common::io::buffer(message));
        return;
      }

      case common_error_t::not_implemented: {
        static std::string const message{"Not implemented\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(501, common::io::buffer(message));
        return;
      }

      case common_error_t::bad_gateway: {
        static std::string const message{"Bad gateway\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(502, common::io::buffer(message));
        return;
      }

      case common_error_t::service_unavailable: {
        static std::string const message{"Service unavailable\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(503, common::io::buffer(message));
        return;
      }

      case common_error_t::gateway_timeout: {
        static std::string const message{"Gateway timeout\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(504, common::io::buffer(message));
        return;
      }

      case common_error_t::http_version_not_supported: {
        static std::string const message{"HTTP version not supported\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(505, common::io::buffer(message));
        return;
      }

      // rfc 7232
      //

      case common_error_t::not_modified: {
        static std::string const message{"Not modified\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(304, common::io::buffer(message));
        return;
      }

      case common_error_t::precondition_failed: {
        static std::string const message{"Precondition failed\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(412, common::io::buffer(message));
        return;
      }

      // rfc 7235
      //

      case common_error_t::unauthorized: {
        static std::string const message{"Unauthorized\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(401, common::io::buffer(message));
        return;
      }

      case common_error_t::proxy_auth_required: {
        static std::string const message{"Proxy authentication required\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(407, common::io::buffer(message));
        return;
      }

      // Unofficial.
      //

      case common_error_t::enhance_your_calm: {
        static std::string const message{"Enhance your calm\n"};

        set<rfc7231::content_type_t>(res, "text", "plain");
        res.send(420, common::io::buffer(message));
        return;
      }
    }

    throw std::invalid_argument{"invalid common error"};
  }

  void
  server_t::
  on_exception(request_t&, response_t&) noexcept
  {
    trace() << "on_exception()...";
  }

  common::diagnostics_t::proxy_t
  server_t::
  trace()
  {
    return diagnostics_ << server_category();
  }

  void
  server_t::
  init_accept(common::io::acceptor_t& acceptor)
  {
    trace() << "init_accept()...";

    auto bound = [this, &acceptor](std::error_code const& ec)
    {
      on_accept(acceptor, ec);
    };

    supervisor_.async_accept(acceptor, bound);
  }

  void
  server_t::
  on_accept(common::io::acceptor_t& acceptor, std::error_code const& ec)
  {
    trace() << "on_accept()...";

    if (!ec) {
      // no error.
      //
      init_accept(acceptor);
    }
    else if (ec != asio::error::operation_aborted) {
      // fixme: handle error accordingly.
      //
    }
  }

} // namespace art::seafire::server
