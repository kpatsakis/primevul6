bool HHVM_FUNCTION(uksort,
                   VRefParam container,
                   const Variant& cmp_function) {
  if (container.isArray()) {
    auto sort = [](Array& arr_array, const Variant& cmp_function) -> bool {
      ArraySortTmp ast(arr_array, SORTFUNC_UKSORT);
      return ast->uksort(cmp_function);
    };
    auto ref = container.getVariantOrNull();
    if (LIKELY(ref != nullptr)) {
      return sort(ref->asArrRef(), cmp_function);
    }
    auto tmp = container->asCArrRef();
    return sort(tmp, cmp_function);
  }
  if (container.isObject()) {
    ObjectData* obj = container.getObjectData();
    if (obj->isCollection()) {
      auto type = obj->collectionType();
      if (type == CollectionType::Map || type == CollectionType::Set) {
        HashCollection* hc = static_cast<HashCollection*>(obj);
        return hc->uksort(cmp_function);
      }
    }
    // other collections are not supported:
    //  - Vectors require a non-associative sort
    //  - Immutable collections are not to be modified
  }
  throw_expected_array_or_collection_exception("uksort");
  return false;
}