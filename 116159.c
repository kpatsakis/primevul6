  static bool isnegative(double x) {
    using namespace fmt::internal;
    if (const_check(sizeof(signbit(x)) != sizeof(fmt::internal::DummyInt))) {
      return signbit(x) != 0;
    }
    if (x < 0) return true;
    if (!isnotanumber(x)) return false;
    int dec = 0, sign = 0;
    char buffer[2];  // The buffer size must be >= 2 or _ecvt_s will fail.
    _ecvt_s(buffer, sizeof(buffer), x, 0, &dec, &sign);
    return sign != 0;
  }