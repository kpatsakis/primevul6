TypedValue HHVM_FUNCTION(array_diff_key,
                         const Variant& container1,
                         const Variant& container2,
                         const Array& args /* = null array */) {
  ARRAY_DIFF_PRELUDE()
  // If we're only dealing with two containers and if they are both arrays,
  // we can avoid creating an intermediate Set
  if (!moreThanTwo && isArrayType(c1.m_type) && isArrayType(c2.m_type)) {
    auto ad2 = c2.m_data.parr;
    for (ArrayIter iter(container1); iter; ++iter) {
      auto key = iter.first();
      const auto& c = *key.asCell();
      if (c.m_type == KindOfInt64) {
        if (ad2->exists(c.m_data.num)) continue;
      } else {
        assert(isStringType(c.m_type));
        if (ad2->exists(c.m_data.pstr)) continue;
      }
      ret.setWithRef(key, iter.secondRefPlus(), true);
    }
    return tvReturn(std::move(ret));
  }
  // Put all of the keys from all the containers (except container1) into a
  // Set. All types aside from integer and string will be cast to string, and
  // we also convert int-like strings to integers.
  auto st = req::make<c_Set>();
  st->reserve(largestSize);
  containerKeysToSetHelper(st, container2);
  if (UNLIKELY(moreThanTwo)) {
    for (ArrayIter argvIter(args); argvIter; ++argvIter) {
      const auto& container = argvIter.secondRef();
      containerKeysToSetHelper(st, container);
    }
  }
  // Loop over container1, only copying over key/value pairs where the key is
  // not present in the Set. When checking if a key is present in the Set, any
  // key that is not an integer or string is cast to a string, and we convert
  // int-like strings to integers.
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  bool isKey = isArrayType(c1.m_type);
  for (ArrayIter iter(container1); iter; ++iter) {
    auto key = iter.first();
    const auto& c = *key.asCell();
    if (checkSetHelper(st, c, strTv, !isKey)) continue;
    ret.setWithRef(key, iter.secondRefPlus(), isKey);
  }
  return tvReturn(std::move(ret));
}