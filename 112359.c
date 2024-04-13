TypedValue HHVM_FUNCTION(array_flip,
                         const Variant& trans) {
  auto const& transCell = *trans.asCell();
  if (UNLIKELY(!isContainer(transCell))) {
    raise_warning("Invalid operand type was used: %s expects "
                  "an array or collection", __FUNCTION__+2);
    return make_tv<KindOfNull>();
  }

  ArrayInit ret(getContainerSize(transCell), ArrayInit::Mixed{});
  for (ArrayIter iter(transCell); iter; ++iter) {
    const Variant& value(iter.secondRefPlus());
    if (value.isString() || value.isInteger()) {
      ret.setUnknownKey(value, iter.first());
    } else {
      raise_warning("Can only flip STRING and INTEGER values!");
    }
  }
  return tvReturn(ret.toVariant());
}