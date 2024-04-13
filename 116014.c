inline uint32_t clzll(uint64_t x) {
  unsigned long r = 0;
# ifdef _WIN64
  _BitScanReverse64(&r, x);
# else
  // Scan the high 32 bits.
  if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
    return 63 - (r + 32);

  // Scan the low 32 bits.
  _BitScanReverse(&r, static_cast<uint32_t>(x));
# endif

  assert(x != 0);
  // Static analysis complains about using uninitialized data
  // "r", but the only way that can happen is if "x" is 0,
  // which the callers guarantee to not happen.
# pragma warning(suppress: 6102)
  return 63 - r;
}