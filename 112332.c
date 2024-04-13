static bool HHVM_METHOD(Memcache, connect, const String& host, int port /*= 0*/,
                        int /*timeout*/ /*= 0*/, int /*timeoutms*/ /*= 0*/) {
  auto data = Native::data<MemcacheData>(this_);
  memcached_return_t ret;

  if (!host.empty() &&
      !strncmp(host.c_str(), "unix://", sizeof("unix://") - 1)) {
    const char *socket_path = host.substr(sizeof("unix://") - 1).c_str();
    ret = memcached_server_add_unix_socket(&data->m_memcache, socket_path);
  } else {
    if (!isServerReachable(host, port)) {
      return false;
    }
    ret = memcached_server_add(&data->m_memcache, host.c_str(), port);
  }

  return (ret == MEMCACHED_SUCCESS);
}