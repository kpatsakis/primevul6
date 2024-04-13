TypedValue HHVM_FUNCTION(array_pad,
                         const Variant& input,
                         int pad_size,
                         const Variant& pad_value) {
  getCheckedArray(input);
  if (pad_size > 0) {
    return tvReturn(ArrayUtil::Pad(arr_input, pad_value, pad_size, true));
  }
  return tvReturn(ArrayUtil::Pad(arr_input, pad_value, -pad_size, false));
}