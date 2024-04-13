static bool HHVM_METHOD(Memcache, close) {
  auto data = Native::data<MemcacheData>(this_);
  memcached_quit(&data->m_memcache);
  return true;
}