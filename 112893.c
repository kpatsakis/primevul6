static TypedValue* serialize_vars_helper(ActRec* ar) {
  WddxPacket* wddxPacket = newres<WddxPacket>(empty_string_variant_ref,
                                              true, true);
  int start_index = 0;
  for (int i = start_index; i < ar->numArgs(); i++) {
    auto const tv = getArg(ar, i);
    find_var_recursive(tv, wddxPacket);
  }
  Variant packet = wddxPacket->packet_end();
  return arReturn(ar, std::move(packet));
}