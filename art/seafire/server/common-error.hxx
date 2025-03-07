#ifndef art__seafire__server__common_error_hxx_
#define art__seafire__server__common_error_hxx_

namespace art::seafire::server
{

  enum class common_error_t
  {
    // RCF 7231.
    //
    bad_request,
    payment_required,
    forbidden,
    not_found,
    method_not_allowed,
    not_acceptable,
    request_timeout,
    conflict,
    gone,
    length_required,
    payload_too_large,
    uri_too_long,
    unsupported_media_type,
    expectation_failed,
    upgrade_required,

    internal_server_error,
    not_implemented,
    bad_gateway,
    service_unavailable,
    gateway_timeout,
    http_version_not_supported,

    // RFC 7232.
    //
    not_modified,
    precondition_failed,

    // RFC 7235.
    //
    unauthorized,
    proxy_auth_required,

    // Unofficial.
    //
    enhance_your_calm

  };

} // namespace art::seafire::server

#endif
