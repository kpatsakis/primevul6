bool WddxPacket::serialize_value(const Variant& varVariant) {
  return recursiveAddVar(empty_string_ref, varVariant, false);
}