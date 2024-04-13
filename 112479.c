TypedValue HHVM_FUNCTION(array_unique,
                         const Variant& array,
                         int sort_flags /* = 2 */) {
  // NOTE, PHP array_unique accepts ArrayAccess objects as well,
  // which is not supported here.
  getCheckedArray(array);
  switch (sort_flags) {
  case SORT_STRING:
  case SORT_LOCALE_STRING:
    return tvReturn(ArrayUtil::StringUnique(arr_array));
  case SORT_NUMERIC:
    return tvReturn(ArrayUtil::NumericUnique(arr_array));
  case SORT_REGULAR:
  default:
    return tvReturn(ArrayUtil::RegularSortUnique(arr_array));
  }
}