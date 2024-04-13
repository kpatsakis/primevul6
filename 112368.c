bool HHVM_FUNCTION(usort,
                   VRefParam container,
                   const Variant& cmp_function) {
  if (container.isArray()) {
    auto sort = [](Array& arr_array, const Variant& cmp_function) -> bool {
      if (RuntimeOption::EnableZendSorting) {
        arr_array.sort(cmp_func, false, true, &cmp_function);
        return true;
      } else {
        ArraySortTmp ast(arr_array, SORTFUNC_USORT);
        return ast->usort(cmp_function);
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
      if (obj->collectionType() == CollectionType::Vector) {
        c_Vector* vec = static_cast<c_Vector*>(obj);
        return vec->usort(cmp_function);
      }
    }
    // other collections are not supported:
    //  - Maps and Sets require associative sort
    //  - Immutable collections are not to be modified
  }
  throw_expected_array_or_collection_exception("usort");
  return false;
}