TypedValue HHVM_FUNCTION(array_replace,
                         const Variant& array1,
                         const Variant& array2 /* = null_variant */,
                         const Array& args /* = null array */) {
  getCheckedArray(array1);
  Array ret = Array::Create();
  php_array_replace(ret, arr_array1);

  if (UNLIKELY(array2.isNull() && args.empty())) {
    return tvReturn(std::move(ret));
  }

  getCheckedArray(array2);
  php_array_replace(ret, arr_array2);

  for (ArrayIter iter(args); iter; ++iter) {
    const Variant& v = iter.secondRef();
    getCheckedArray(v);
    php_array_replace(ret, arr_v);
  }
  return tvReturn(std::move(ret));
}