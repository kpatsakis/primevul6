Variant HHVM_FUNCTION(current,
                      VRefParam refParam) {
  return iter_op_impl<NoCow>(
    refParam,
    &ArrayData::current,
    s___current,
    false
  );
}