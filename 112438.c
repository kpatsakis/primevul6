TypedValue HHVM_FUNCTION(array_shift,
                         VRefParam array) {
  const auto* cell_array = array->asCell();
  if (UNLIKELY(!isMutableContainer(*cell_array))) {
    raise_warning("array_shift() expects parameter 1 to be an "
                  "array or mutable collection");
    return make_tv<KindOfNull>();
  }
  if (!getContainerSize(array)) {
    return make_tv<KindOfNull>();
  }
  if (isArrayType(cell_array->m_type)) {
    if (auto ref = array.getVariantOrNull()) {
      return tvReturn(ref->asArrRef().dequeue());
    }
    auto ad = cell_array->m_data.parr;
    if (ad->size()) {
      auto first = ad->iter_begin();
      return tvReturn(ad->getValue(first));
    }
    return make_tv<KindOfNull>();
  }
  assertx(cell_array->m_type == KindOfObject);
  return tvReturn(collections::shift(cell_array->m_data.pobj));
}