#include <art/seafire/server/diagnostics.hxx>

namespace art::seafire::server
{

  common::diagnostics_t::category_t const&
  server_category()
  {
    static common::diagnostics_t::category_t category{"server"};
    return category;
  }

  common::diagnostics_t::category_t const&
  supervisor_category()
  {
    static common::diagnostics_t::category_t category{"supervisor"};
    return category;
  }

  common::diagnostics_t::category_t const&
  session_category()
  {
    static common::diagnostics_t::category_t category{"session"};
    return category;
  }

  common::diagnostics_t::category_t const&
  transaction_category()
  {
    static common::diagnostics_t::category_t category{"transaction"};
    return category;
  }

  common::diagnostics_t::category_t const&
  request_category()
  {
    static common::diagnostics_t::category_t category{"request"};
    return category;
  }

} // namespace art::seafire::server
