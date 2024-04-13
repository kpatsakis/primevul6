static void containerKeysIntersectHelper(const req::ptr<c_Set>& st,
                                         TypedValue* containers,
                                         int count) {
  assert(count >= 2);
  auto mp = req::make<c_Map>();
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  TypedValue intOneTv = make_tv<KindOfInt64>(1);
  bool isKey = isArrayType(containers[0].m_type);
  for (ArrayIter iter(tvAsCVarRef(&containers[0])); iter; ++iter) {
    auto key = iter.first();
    const auto& c = *key.asCell();
    // For each key k in containers[0], we add the key/value pair (k, 1)
    // to the map. If a key (after various conversions) occurs more than
    // once in the container, we'll simply overwrite the old entry and
    // that's fine.
    addToIntersectMapHelper(mp, c, &intOneTv, strTv, !isKey);
  }
  for (int pos = 1; pos < count; ++pos) {
    isKey = isArrayType(containers[pos].m_type);
    for (ArrayIter iter(tvAsCVarRef(&containers[pos])); iter; ++iter) {
      auto key = iter.first();
      const auto& c = *key.asCell();
      updateIntersectMapHelper(mp, c, pos, strTv, !isKey);
    }
  }
  for (ArrayIter iter(mp.get()); iter; ++iter) {
    // For each key in the map, we copy the key to the set if the
    // corresponding value is equal to pos exactly (which means it
    // was present in all of the containers).
    const auto& val = *iter.secondRefPlus().asCell();
    assert(val.m_type == KindOfInt64);
    if (val.m_data.num == count) {
      st->add(iter.first().asCell());
    }
  }
}