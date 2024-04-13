static void php_array_replace(Array &arr1, const Array& arr2) {
  for (ArrayIter iter(arr2); iter; ++iter) {
    Variant key = iter.first();
    const Variant& value = iter.secondRef();
    arr1.setWithRef(key, value, true);
  }
}