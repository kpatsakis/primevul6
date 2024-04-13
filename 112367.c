TypedValue HHVM_FUNCTION(hphp_array_idx,
                         const Variant& search,
                         const Variant& key,
                         const Variant& def) {
  if (!key.isNull()) {
    if (LIKELY(search.isArray())) {
      ArrayData *arr = search.getArrayData();
      VarNR index = key.toKey(arr);
      if (!index.isNull()) {
        const Variant& ret = arr->get(index, false);
        return tvReturn((&ret != &null_variant) ? ret : def);
      }
    } else {
      raise_error("hphp_array_idx: search must be an array");
    }
  }
  return tvReturn(def);
}