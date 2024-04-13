TypedValue HHVM_FUNCTION(array_splice,
                         VRefParam input,
                         int offset,
                         const Variant& length,
                         const Variant& replacement) {
  return tvReturn(array_splice(input, offset, length, replacement));
}