Variant HHVM_FUNCTION(each,
                      VRefParam refParam) {
  return iter_op_impl(
    refParam,
    &ArrayData::each,
    s___each,
    Variant::NullInit()
  );
}