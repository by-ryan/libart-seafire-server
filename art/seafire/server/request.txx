namespace art::seafire::server
{

  template<typename Header>
  bool
  has(request_t const& r)
  {
    return protocol::has<Header>(r.get_message());
  }

  template<typename Header>
  bool
  has_quick(request_t const& r)
  {
    return protocol::has_quick<Header>(r.get_message());
  }

  template<typename Header>
  auto
  get(request_t const& r)
  {
    return protocol::get<Header>(r.get_message());
  }

  template<typename Header>
  auto
  get(request_t const& r, std::error_code& ec)
  {
    return protocol::get<Header>(r.get_message(), ec);
  }

} // namespace art::seafire::server
