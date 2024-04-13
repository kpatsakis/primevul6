void find_var_recursive(const TypedValue* tv, WddxPacket* wddxPacket) {
  if (tvIsString(tv)) {
    String var_name = tvCastToString(tv);
    wddxPacket->add_var(var_name, true);
  }
  if (tv->m_type == KindOfArray) {
    for (ArrayIter iter(tv->m_data.parr); iter; ++iter) {
      find_var_recursive(iter.secondRef().asTypedValue(), wddxPacket);
    }
  }
}