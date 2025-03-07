#ifndef art__seafire__server__configuration_hxx_
#define art__seafire__server__configuration_hxx_

#include <chrono>

namespace art::seafire::server
{

  /// Holds server configuration parameters.
  ///
  struct configuration_t
  {
    /// The timeout of a transaction.
    ///
    std::chrono::seconds request_timeout;

  };

} // namespace art::seafire::server

#endif
