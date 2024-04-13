TypedValue HHVM_FUNCTION(array_fill_keys,
                         const Variant& keys,
                         const Variant& value) {
  folly::Optional<ArrayInit> ai;
  auto ok = IterateV(*keys.asCell(),
                     [&](ArrayData* adata) {
                       ai.emplace(adata->size(), ArrayInit::Mixed{});
                     },
                     [&](const TypedValue* tv) {
                       auto& key = tvAsCVarRef(tv);
                       if (key.isInteger() || key.isString()) {
                         ai->setUnknownKey(key, value);
                       } else {
                         raise_hack_strict(RuntimeOption::StrictArrayFillKeys,
                                           "strict_array_fill_keys",
                                           "keys must be ints or strings");
                         ai->setUnknownKey(key.toString(), value);
                       }
                     },
                     [&](ObjectData* coll) {
                       if (coll->collectionType() == CollectionType::Pair) {
                         ai.emplace(2, ArrayInit::Mixed{});
                       }
                     });

  if (!ok) {
    raise_warning("Invalid operand type was used: array_fill_keys expects "
                  "an array or collection");
    return make_tv<KindOfNull>();
  }
  assert(ai.hasValue());
  return tvReturn(ai->toVariant());
}