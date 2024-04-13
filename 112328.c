HHVM_METHOD(Memcache, addserver, const String& host, int port /* = 11211 */,
            bool /*persistent*/ /* = false */, int weight /* = 0 */,
            int /*timeout*/ /* = 0 */, int /*retry_interval*/ /* = 0 */,
            bool /*status*/ /* = true */,
            const Variant& /*failure_callback*/ /* = uninit_variant */,
            int /*timeoutms*/ /* = 0 */) {
  auto data = Native::data<MemcacheData>(this_);
  memcached_return_t ret;

  if (!host.empty() &&
      !strncmp(host.c_str(), "unix://", sizeof("unix://") - 1)) {
    const char *socket_path = host.substr(sizeof("unix://") - 1).c_str();
    ret = memcached_server_add_unix_socket_with_weight(&data->m_memcache,
                                                       socket_path, weight);
  } else {
    ret = memcached_server_add_with_weight(&data->m_memcache, host.c_str(),
                                           port, weight);
  }

  if (ret == MEMCACHED_SUCCESS) {
    return true;
  }

  return false;
}