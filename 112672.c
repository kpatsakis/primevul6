static bool isUtf8(const Variant& encoding) {
  return encoding.getStringDataOrNull() == s_utf_8.get();
}