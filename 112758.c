String string_addslashes(const char *str, int length) {
  assert(str);
  if (length == 0) {
    return String();
  }

  String retString((length << 1) + 1, ReserveString);
  char *new_str = retString.bufferSlice().ptr;
  const char *source = str;
  const char *end = source + length;
  char *target = new_str;

  while (source < end) {
    switch (*source) {
    case '\0':
      *target++ = '\\';
      *target++ = '0';
      break;
    case '\'':
    case '\"':
    case '\\':
      *target++ = '\\';
      /* break is missing *intentionally* */
    default:
      *target++ = *source;
      break;
    }

    source++;
  }

  retString.setSize(target - new_str);
  return retString;
}