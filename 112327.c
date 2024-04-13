static bool HHVM_METHOD(Memcache, flush, int expire /*= 0*/) {
  auto data = Native::data<MemcacheData>(this_);
  return memcached_flush(&data->m_memcache, expire) == MEMCACHED_SUCCESS;
}