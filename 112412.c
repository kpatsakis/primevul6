TypedValue* HHVM_FN(array_multisort)(ActRec* ar) {
  TypedValue* tv = getArg(ar, 0);
  if (tv == nullptr || !tvAsVariant(tv).isArray()) {
    throw_expected_array_exception("array_multisort");
    return arReturn(ar, false);
  }

  std::vector<Array::SortData> data;
  std::vector<Array> arrays;
  arrays.reserve(ar->numArgs()); // so no resize would happen

  Array::SortData sd;
  sd.original = &tvAsVariant(tv);
  arrays.push_back(Array(sd.original->getArrayData()));
  sd.array = &arrays.back();
  sd.by_key = false;

  int sort_flags = SORT_REGULAR;
  bool ascending = true;
  for (int i = 1; i < ar->numArgs(); i++) {
    tv = getArg(ar, i);
    if (tvAsVariant(tv).isArray()) {
      sd.cmp_func = get_cmp_func(sort_flags, ascending);
      data.push_back(sd);

      sort_flags = SORT_REGULAR;
      ascending = true;

      sd.original = &tvAsVariant(tv);
      arrays.push_back(Array(sd.original->getArrayData()));
      sd.array = &arrays.back();
    } else {
      int n = toInt32(getArg<KindOfInt64>(ar, i));
      if (n == SORT_ASC) {
      } else if (n == SORT_DESC) {
        ascending = false;
      } else {
        sort_flags = n;
      }
    }
  }

  sd.cmp_func = get_cmp_func(sort_flags, ascending);
  data.push_back(sd);

  return arReturn(ar, Array::MultiSort(data, true));
}