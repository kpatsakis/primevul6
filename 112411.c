int64_t HHVM_FUNCTION(sizeof,
                      const Variant& var,
                      int64_t mode /* = 0 */) {
  return HHVM_FN(count)(var, mode);
}