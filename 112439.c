TypedValue HHVM_FUNCTION(array_merge_recursive,
                         int64_t numArgs,
                         const Variant& array1,
                         const Variant& array2 /* = null_variant */,
                         const Array& args /* = null array */) {
  getCheckedArray(array1);
  auto in1 = array1.asCArrRef();
  auto ret = Array::attach(MixedArray::MakeReserveLike(in1.get(), 0));
  PointerSet seen;
  php_array_merge_recursive(seen, false, ret, arr_array1);
  assert(seen.empty());

  if (UNLIKELY(numArgs < 2)) return tvReturn(std::move(ret));

  getCheckedArray(array2);
  php_array_merge_recursive(seen, false, ret, arr_array2);
  assert(seen.empty());

  for (ArrayIter iter(args); iter; ++iter) {
    Variant v = iter.second();
    if (!v.isArray()) {
      throw_expected_array_exception("array_merge_recursive");
      return make_tv<KindOfNull>();
    }
    const Array& arr_v = v.asCArrRef();
    php_array_merge_recursive(seen, false, ret, arr_v);
    assert(seen.empty());
  }
  return tvReturn(std::move(ret));
}