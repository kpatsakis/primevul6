static bool isServerReachable(const String& host, int port /*= 0*/) {
  auto hostInfo = HHVM_FN(getaddrinfo)(host, port);
  if (hostInfo.isBoolean() && !hostInfo.toBoolean()) {
    raise_warning("Memcache: Can't connect to %s:%d", host.c_str(), port);
    return false;
  }
  return true;
}