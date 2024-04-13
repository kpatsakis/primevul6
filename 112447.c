TypedValue HHVM_FUNCTION(array_combine,
                         const Variant& keys,
                         const Variant& values) {
  const auto& cell_keys = *keys.asCell();
  const auto& cell_values = *values.asCell();
  if (UNLIKELY(!isContainer(cell_keys) || !isContainer(cell_values))) {
    raise_warning("Invalid operand type was used: array_combine expects "
                  "arrays or collections");
    return make_tv<KindOfNull>();
  }
  auto keys_size = getContainerSize(cell_keys);
  if (UNLIKELY(keys_size != getContainerSize(cell_values))) {
    raise_warning("array_combine(): Both parameters should have an equal "
                  "number of elements");
    return make_tv<KindOfBoolean>(false);
  }
  Array ret = Array::attach(MixedArray::MakeReserveMixed(keys_size));
  for (ArrayIter iter1(cell_keys), iter2(cell_values);
       iter1; ++iter1, ++iter2) {
    const Variant& key = iter1.secondRefPlus();
    if (key.isInteger() || key.isString()) {
      ret.setWithRef(key, iter2.secondRefPlus());
    } else {
      ret.setWithRef(key.toString(), iter2.secondRefPlus());
    }
  }
  return tvReturn(std::move(ret));
}