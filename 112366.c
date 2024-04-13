Variant HHVM_FUNCTION(next,
                      VRefParam refParam) {
  return iter_op_impl(
    refParam,
    &ArrayData::next,
    s___next,
    false
  );
}