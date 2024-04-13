Variant array_values(const Variant& input) {
  if (input.isArray() && input.asCArrRef()->isVecArray()) {
    return input;
  }

  folly::Optional<PackedArrayInit> ai;
  auto ok = IterateV(*input.asCell(),
                     [&](ArrayData* adata) {
                       ai.emplace(adata->size());
                     },
                     [&](const TypedValue* tv) {
                       ai->appendWithRef(tvAsCVarRef(tv));
                     },
                     [&](ObjectData* coll) {
                       if (coll->collectionType() == CollectionType::Pair) {
                         ai.emplace(2);
                       }
                     });

  if (!ok) {
    raise_warning("array_values() expects parameter 1 to be an array "
                  "or collection");
    return init_null();
  }

  assert(ai.hasValue());
  return ai->toVariant();
}