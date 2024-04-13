TypedValue HHVM_FUNCTION(array_pop,
                         VRefParam containerRef) {
  const auto* container = containerRef->asCell();
  if (UNLIKELY(!isMutableContainer(*container))) {
    raise_warning("array_pop() expects parameter 1 to be an "
                  "array or mutable collection");
    return make_tv<KindOfNull>();
  }
  if (!getContainerSize(containerRef)) {
    return make_tv<KindOfNull>();
  }
  if (isArrayType(container->m_type)) {
    if (auto ref = containerRef.getVariantOrNull()) {
      return tvReturn(ref->asArrRef().pop());
    }
    auto ad = container->m_data.parr;
    if (ad->size()) {
      auto last = ad->iter_last();
      return tvReturn(ad->getValue(last));
    }
    return make_tv<KindOfNull>();
  }
  assert(container->m_type == KindOfObject);
  return tvReturn(collections::pop(container->m_data.pobj));
}