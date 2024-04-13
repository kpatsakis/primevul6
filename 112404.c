static Array::PFUNC_CMP get_cmp_func(int sort_flags, bool ascending) {
  switch (sort_flags) {
  case SORT_NATURAL:
    return ascending ?
      Array::SortNaturalAscending : Array::SortNaturalDescending;
  case SORT_NATURAL_CASE:
    return ascending ?
      Array::SortNaturalCaseAscending: Array::SortNaturalCaseDescending;
  case SORT_NUMERIC:
    return ascending ?
      Array::SortNumericAscending : Array::SortNumericDescending;
  case SORT_STRING:
    return ascending ?
      Array::SortStringAscending : Array::SortStringDescending;
  case SORT_STRING_CASE:
    return ascending ?
      Array::SortStringAscendingCase : Array::SortStringDescendingCase;
  case SORT_LOCALE_STRING:
    return ascending ?
      Array::SortLocaleStringAscending : Array::SortLocaleStringDescending;
  case SORT_REGULAR:
  default:
    return ascending ?
      Array::SortRegularAscending : Array::SortRegularDescending;
  }
}