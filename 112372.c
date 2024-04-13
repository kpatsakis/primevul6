TypedValue HHVM_FUNCTION(array_intersect_key,
                         const Variant& container1,
                         const Variant& container2,
                         const Array& args /* = null array */) {
  ARRAY_INTERSECT_PRELUDE()
  // If we're only dealing with two containers and if they are both arrays,
  // we can avoid creating an intermediate Set
  if (!moreThanTwo && isArrayType(c1.m_type) && isArrayType(c2.m_type)) {
    auto ad2 = c2.m_data.parr;
    for (ArrayIter iter(container1); iter; ++iter) {
      auto key = iter.first();
      const auto& c = *key.asCell();
      if (c.m_type == KindOfInt64) {
        if (!ad2->exists(c.m_data.num)) continue;
      } else {
        assert(isStringType(c.m_type));
        if (!ad2->exists(c.m_data.pstr)) continue;
      }
      ret.setWithRef(key, iter.secondRefPlus(), true);
    }
    return tvReturn(std::move(ret));
  }
  // Build up a Set containing the keys that are present in all the containers
  // (except container1)
  auto st = req::make<c_Set>();
  if (LIKELY(!moreThanTwo)) {
    // There is only one container (not counting container1) so we can just
    // call containerKeysToSetHelper() to build the Set.
    containerKeysToSetHelper(st, container2);
  } else {
    // We're dealing with three or more containers. Copy all of the containers
    // (except the first) into a TypedValue array.
    int count = args.size() + 1;
    TypedValue* containers =
      makeContainerListHelper(container2, args, count, smallestPos);
    SCOPE_EXIT { req::free(containers); };
    // Build a Set of the keys that were present in all of the containers
    containerKeysIntersectHelper(st, containers, count);
  }
  // Loop over container1, only copying over key/value pairs where the key
  // is present in the Set. When checking if a key is present in the Set,
  // any value that is not an integer or string is cast to a string, and we
  // convert int-like strings to integers.
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  bool isKey = isArrayType(c1.m_type);
  for (ArrayIter iter(container1); iter; ++iter) {
    auto key = iter.first();
    const auto& c = *key.asCell();
    if (!checkSetHelper(st, c, strTv, !isKey)) continue;
    ret.setWithRef(key, iter.secondRefPlus(), isKey);
  }
  return tvReturn(std::move(ret));
}