Variant array_splice(VRefParam input, int offset,
                     const Variant& length, const Variant& replacement) {
  getCheckedArrayVariant(input);
  Array ret(Array::Create());
  int64_t len = length.isNull() ? 0x7FFFFFFF : length.toInt64();
  input.assignIfRef(ArrayUtil::Splice(arr_input, offset, len, replacement, &ret));
  return ret;
}