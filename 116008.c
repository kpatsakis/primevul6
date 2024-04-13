  static bool isnotanumber(T x) {
    using namespace fmt::internal;
    if (const_check(sizeof(isnan(x)) != sizeof(fmt::internal::DummyInt))) {
      return isnan(x) != 0;
    }
    return _isnan(static_cast<double>(x)) != 0;
  }