TypedValue HHVM_FUNCTION(array_rand,
                         const Variant& input,
                         int num_req /* = 1 */) {
  getCheckedArray(input);
  return tvReturn(ArrayUtil::RandomKeys(arr_input, num_req));
}