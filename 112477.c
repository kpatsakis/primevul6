static void containerValuesIntersectHelper(const req::ptr<c_Set>& st,
                                           TypedValue* containers,
                                           int count) {
  assert(count >= 2);
  auto mp = req::make<c_Map>();
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  TypedValue intOneTv = make_tv<KindOfInt64>(1);
  for (ArrayIter iter(tvAsCVarRef(&containers[0])); iter; ++iter) {
    const auto& c = *iter.secondRefPlus().asCell();
    // For each value v in containers[0], we add the key/value pair (v, 1)
    // to the map. If a value (after various conversions) occurs more than
    // once in the container, we'll simply overwrite the old entry and that's
    // fine.
    addToIntersectMapHelper(mp, c, &intOneTv, strTv, true);
  }
  for (int pos = 1; pos < count; ++pos) {
    for (ArrayIter iter(tvAsCVarRef(&containers[pos])); iter; ++iter) {
      const auto& c = *iter.secondRefPlus().asCell();
      // We check if the value is present as a key in the map. If an entry
      // exists and its value equals pos, we increment it, otherwise we do
      // nothing. This is essential so that we don't accidentally double-count
      // a key (after various conversions) that occurs in the container more
      // than once.
      updateIntersectMapHelper(mp, c, pos, strTv, true);
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