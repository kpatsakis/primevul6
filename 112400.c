Variant HHVM_FUNCTION(end,
                      VRefParam refParam) {
  return iter_op_impl(
    refParam,
    &ArrayData::end,
    s___end,
    false,
    &ArrayData::isTail
  );
}