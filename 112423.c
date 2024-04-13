static void containerValuesToSetHelper(const req::ptr<c_Set>& st,
                                       const Variant& container) {
  Variant strHolder(empty_string_variant());
  TypedValue* strTv = strHolder.asTypedValue();
  for (ArrayIter iter(container); iter; ++iter) {
    auto const& c = *iter.secondRefPlus().asCell();
    addToSetHelper(st, c, strTv, true);
  }
}