php_sort(VRefParam container, int sort_flags,
         bool ascending, bool use_zend_sort) {
  if (container.isArray()) {
    auto ref = container.getVariantOrNull();
    if (!ref) return true;
    Array& arr_array = ref->asArrRef();
    if (use_zend_sort) {
      return zend_sort(*ref, sort_flags, ascending);
    }
    SortFunction sf = getSortFunction(SORTFUNC_SORT, ascending);
    ArraySortTmp ast(arr_array, sf);
    ast->sort(sort_flags, ascending);
    return true;
  }
  if (container.isObject()) {
    ObjectData* obj = container.getObjectData();
    if (obj->isCollection() &&
        obj->collectionType() == CollectionType::Vector) {
      c_Vector* vec = static_cast<c_Vector*>(obj);
      vec->sort(sort_flags, ascending);
      return true;
    }
    // other collections are not supported:
    //  - Maps and Sets require associative sort
    //  - Immutable collections are not to be modified
  }
  throw_expected_array_or_collection_exception(ascending ? "sort" : "rsort");
  return false;
}