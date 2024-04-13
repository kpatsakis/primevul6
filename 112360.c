TypedValue HHVM_FUNCTION(array_replace_recursive,
                         const Variant& array1,
                         const Variant& array2 /* = null_variant */,
                         const Array& args /* = null array */) {
  getCheckedArray(array1);
  Array ret = Array::Create();
  PointerSet seen;
  php_array_replace_recursive(seen, false, ret, arr_array1);
  assert(seen.empty());

  if (UNLIKELY(array2.isNull() && args.empty())) {
    return tvReturn(std::move(ret));
  }

  getCheckedArray(array2);
  php_array_replace_recursive(seen, false, ret, arr_array2);
  assert(seen.empty());

  for (ArrayIter iter(args); iter; ++iter) {
    const Variant& v = iter.secondRef();
    getCheckedArray(v);
    php_array_replace_recursive(seen, false, ret, arr_v);
    assert(seen.empty());
  }
  return tvReturn(std::move(ret));
}