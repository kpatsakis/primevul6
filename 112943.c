bool HHVM_FUNCTION(hash_equals, const Variant& known, const Variant& user) {
  if (!known.isString()) {
    raise_warning(
      "hash_equals(): Expected known_string to be a string, %s given",
      getDataTypeString(known.getType()).c_str()
    );
    return false;
  }
  if (!user.isString()) {
    raise_warning(
      "hash_equals(): Expected user_string to be a string, %s given",
      getDataTypeString(user.getType()).c_str()
    );
    return false;
  }
  String known_str = known.toString();
  String user_str = user.toString();
  const auto known_len = known_str.size();
  const auto known_limit = known_len - 1;
  const auto user_len = user_str.size();
  int64_t result = known_len ^ user_len;

  int64_t ki = 0;
  for (int64_t ui = 0; ui < user_len; ++ui) {
    result |= user_str[ui] ^ known_str[ki];
    if (ki < known_limit) {
      ++ki;
    }
  }
  return (result == 0);
}