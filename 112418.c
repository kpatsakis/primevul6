bool HHVM_FUNCTION(uasort,
                   VRefParam container,
                   const Variant& cmp_function) {
  if (container.isArray()) {
    auto sort = [](Array& arr_array, const Variant& cmp_function) -> bool {
      if (RuntimeOption::EnableZendSorting) {
        arr_array.sort(cmp_func, false, false, &cmp_function);
        return true;
      } else {
        ArraySortTmp ast(arr_array, SORTFUNC_UASORT);
        return ast->uasort(cmp_function);
      }
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
        return hc->uasort(cmp_function);
      }
    }
    // other collections are not supported:
    //  - Vectors require a non-associative sort
    //  - Immutable collections are not to be modified
  }
  throw_expected_array_or_collection_exception("uasort");
  return false;
}