php_ksort(VRefParam container, int sort_flags, bool ascending,
          bool use_zend_sort) {
  if (container.isArray()) {
    auto ref = container.getVariantOrNull();
    if (!ref) return true;
    Array& arr_array = ref->asArrRef();
    if (use_zend_sort) {
      return zend_ksort(*ref, sort_flags, ascending);
    }
    SortFunction sf = getSortFunction(SORTFUNC_KRSORT, ascending);
    ArraySortTmp ast(arr_array, sf);
    ast->ksort(sort_flags, ascending);
    return true;
  }
  if (container.isObject()) {
    ObjectData* obj = container.getObjectData();
    if (obj->isCollection()) {
      auto type = obj->collectionType();
      if (type == CollectionType::Map || type == CollectionType::Set) {
        HashCollection* hc = static_cast<HashCollection*>(obj);
        hc->ksort(sort_flags, ascending);
        return true;
      }
    }
  }
  throw_expected_array_or_collection_exception(ascending ? "ksort" : "krsort");
  return false;
}