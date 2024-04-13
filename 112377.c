TypedValue HHVM_FUNCTION(array_diff,
                         const Variant& container1,
                         const Variant& container2,
                         const Array& args /* = null array */) {
  ARRAY_DIFF_PRELUDE()
  // Put all of the values from all the containers (except container1 into a
  // Set. All types aside from integer and string will be cast to string, and
  // we also convert int-like strings to integers.
  auto st = req::make<c_Set>();
  st->reserve(largestSize);
  containerValuesToSetHelper(st, container2);
  if (UNLIKELY(moreThanTwo)) {
    for (ArrayIter argvIter(args); argvIter; ++argvIter) {
      const auto& container = argvIter.secondRef();
      containerValuesToSetHelper(st, container);
    }
  }
  // Loop over container1, only copying over key/value pairs where the value
  // is not present in the Set. When checking if a value is present in the
  // Set, any value that is not an integer or string is cast to a string, and
  // we convert int-like strings to integers.
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  bool isKey = isArrayType(c1.m_type);
  for (ArrayIter iter(container1); iter; ++iter) {
    const auto& val = iter.secondRefPlus();
    const auto& c = *val.asCell();
    if (checkSetHelper(st, c, strTv, true)) continue;
    ret.setWithRef(iter.first(), val, isKey);
  }
  return tvReturn(std::move(ret));
}