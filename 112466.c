Variant HHVM_FUNCTION(reset,
                      VRefParam refParam) {
  return iter_op_impl(
    refParam,
    &ArrayData::reset,
    s___reset,
    false,
    &ArrayData::isHead
  );
}