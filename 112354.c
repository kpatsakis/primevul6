php_asort(VRefParam container, int sort_flags,
          bool ascending, bool use_zend_sort) {
  if (container.isArray()) {
    auto ref = container.getVariantOrNull();
    if (!ref) return true;
    Array& arr_array = ref->asArrRef();
    if (use_zend_sort) {
      return zend_asort(*ref, sort_flags, ascending);
    }
    SortFunction sf = getSortFunction(SORTFUNC_ASORT, ascending);
    ArraySortTmp ast(arr_array, sf);
    ast->asort(sort_flags, ascending);
    return true;
  }
  if (container.isObject()) {
    ObjectData* obj = container.getObjectData();
    if (obj->isCollection()) {
      auto type = obj->collectionType();
      if (type == CollectionType::Map || type == CollectionType::Set) {
        HashCollection* hc = static_cast<HashCollection*>(obj);
        hc->asort(sort_flags, ascending);
        return true;
      }
    }
  }
  throw_expected_array_or_collection_exception(ascending ? "asort" : "arsort");
  return false;
}