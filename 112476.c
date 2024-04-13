Variant HHVM_FUNCTION(prev,
                      VRefParam refParam) {
  return iter_op_impl(
    refParam,
    &ArrayData::prev,
    s___prev,
    false
  );
}