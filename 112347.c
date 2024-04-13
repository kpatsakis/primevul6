static bool HHVM_METHOD(Memcache, setcompressthreshold, int threshold,
                                        double min_savings /* = 0.2 */) {
  if (threshold < 0) {
    raise_warning("threshold must be a positive integer");
    return false;
  }

  if (min_savings < 0 || min_savings > 1) {
    raise_warning("min_savings must be a float in the 0..1 range");
    return false;
  }

  auto data = Native::data<MemcacheData>(this_);

  data->m_compress_threshold = threshold;
  data->m_min_compress_savings = min_savings;

  return true;
}