Variant HHVM_FUNCTION(key,
                      VRefParam refParam) {
  return iter_op_impl<NoCow>(
    refParam,
    &ArrayData::key,
    s___key,
    false
  );
}