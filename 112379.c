static void containerKeysToSetHelper(const req::ptr<c_Set>& st,
                                     const Variant& container) {
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  bool isKey = isArrayType(container.asCell()->m_type);
  for (ArrayIter iter(container); iter; ++iter) {
    addToSetHelper(st, *iter.first().asCell(), strTv, !isKey);
  }
}