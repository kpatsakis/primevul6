String string_replace(const char *s, int len, int start, int length,
                      const char *replacement, int len_repl) {
  assert(s);
  assert(replacement);
  assert(len >= 0);

  // if "start" position is negative, count start position from the end
  // of the string
  if (start < 0) {
    start = len + start;
    if (start < 0) {
      start = 0;
    }
  }
  if (start > len) {
    start = len;
  }
  // if "length" position is negative, set it to the length
  // needed to stop that many chars from the end of the string
  if (length < 0) {
    length = (len - start) + length;
    if (length < 0) {
      length = 0;
    }
  }
  // check if length is too large
  if (length > len) {
    length = len;
  }
  // check if the length is too large adjusting for non-zero start
  // Write this way instead of start + length > len to avoid overflow
  if (length > len - start) {
    length = len - start;
  }

  String retString(len + len_repl - length, ReserveString);
  char *ret = retString.bufferSlice().ptr;

  int ret_len = 0;
  if (start) {
    memcpy(ret, s, start);
    ret_len += start;
  }
  if (len_repl) {
    memcpy(ret + ret_len, replacement, len_repl);
    ret_len += len_repl;
  }
  len -= (start + length);
  if (len) {
    memcpy(ret + ret_len, s + start + length, len);
    ret_len += len;
  }
  retString.setSize(ret_len);
  return retString;
}