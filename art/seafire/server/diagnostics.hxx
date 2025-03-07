#ifndef art__seafire__server__diagnostics_hxx_
#define art__seafire__server__diagnostics_hxx_

#include <art/seafire/common/diagnostics.hxx>

namespace art::seafire::server
{

  common::diagnostics_t::category_t const&
  server_category();

  common::diagnostics_t::category_t const&
  supervisor_category();

  common::diagnostics_t::category_t const&
  session_category();

  common::diagnostics_t::category_t const&
  transaction_category();

  common::diagnostics_t::category_t const&
  request_category();

} // namespace art::seafire::server

#endif
