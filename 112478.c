TypedValue HHVM_FUNCTION(array_column,
                         ArrayArg input,
                         const Variant& val_key,
                         const Variant& idx_key /* = null_variant */) {

  ArrNR arr_input(input.get());
  Variant val = val_key, idx = idx_key;
  if (!array_column_coerce_key(val, "column") ||
      !array_column_coerce_key(idx, "index")) {
    return make_tv<KindOfBoolean>(false);
  }
  ArrayInit ret(input->size(), ArrayInit::Map{});
  for(auto it = arr_input.asArray().begin(); !it.end(); it.next()) {
    if (!it.second().isArray()) {
      continue;
    }
    Array sub = it.second().toArray();

    Variant elem;
    if (val.isNull()) {
      elem = sub;
    } else if (sub.exists(val)) {
      elem = sub[val];
    } else {
      // skip subarray without named element
      continue;
    }

    if (idx.isNull() || !sub.exists(idx)) {
      ret.append(elem);
    } else if (sub[idx].isObject()) {
      ret.setUnknownKey(sub[idx].toString(), elem);
    } else {
      ret.setUnknownKey(sub[idx], elem);
    }
  }
  return tvReturn(ret.toVariant());
}