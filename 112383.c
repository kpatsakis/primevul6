TypedValue HHVM_FUNCTION(array_intersect,
                         const Variant& container1,
                         const Variant& container2,
                         const Array& args /* = null array */) {
  ARRAY_INTERSECT_PRELUDE()
  // Build up a Set containing the values that are present in all the
  // containers (except container1)
  auto st = req::make<c_Set>();
  if (LIKELY(!moreThanTwo)) {
    // There is only one container (not counting container1) so we can
    // just call containerValuesToSetHelper() to build the Set.
    containerValuesToSetHelper(st, container2);
  } else {
    // We're dealing with three or more containers. Copy all of the containers
    // (except the first) into a TypedValue array.
    int count = args.size() + 1;
    TypedValue* containers =
      makeContainerListHelper(container2, args, count, smallestPos);
    SCOPE_EXIT { req::free(containers); };
    // Build a Set of the values that were present in all of the containers
    containerValuesIntersectHelper(st, containers, count);
  }
  // Loop over container1, only copying over key/value pairs where the value
  // is present in the Set. When checking if a value is present in the Set,
  // any value that is not an integer or string is cast to a string, and we
  // convert int-like strings to integers.
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  bool isKey = isArrayType(c1.m_type);
  for (ArrayIter iter(container1); iter; ++iter) {
    const auto& val = iter.secondRefPlus();
    const auto& c = *val.asCell();
    if (!checkSetHelper(st, c, strTv, true)) continue;
    ret.setWithRef(iter.first(), val, isKey);
  }
  return tvReturn(std::move(ret));
}