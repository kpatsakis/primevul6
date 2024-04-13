TypedValue HHVM_FUNCTION(array_merge,
                         int64_t numArgs,
                         ArrayArg array1,
                         const Variant& array2 /* = null_variant */,
                         const Array& args /* = null array */) {
  Array ret = Array::attach(MixedArray::MakeReserveLike(array1.get(), 0));
  php_array_merge(ret, ArrNR(array1.get()));

  if (UNLIKELY(numArgs < 2)) return tvReturn(std::move(ret));

  getCheckedArrayRet(array2, make_tv<KindOfNull>());
  php_array_merge(ret, arr_array2);

  for (ArrayIter iter(args); iter; ++iter) {
    Variant v = iter.second();
    if (!v.isArray()) {
      throw_expected_array_exception("array_merge");
      return make_tv<KindOfNull>();
    }
    const Array& arr_v = v.asCArrRef();
    php_array_merge(ret, arr_v);
  }
  return tvReturn(std::move(ret));
}