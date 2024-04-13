static int php_count_recursive(const Array& array) {
  long cnt = array.size();
  for (ArrayIter iter(array); iter; ++iter) {
    Variant value = iter.second();
    if (value.isArray()) {
      const Array& arr_value = value.asCArrRef();
      check_recursion_throw();
      cnt += php_count_recursive(arr_value);
    }
  }
  return cnt;
}