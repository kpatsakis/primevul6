String string_replace(const char *input, int len,
                      const char *search, int len_search,
                      const char *replacement, int len_replace,
                      int &count, bool case_sensitive) {
  assert(input);
  assert(search && len_search);
  assert(len >= 0);
  assert(len_search >= 0);
  assert(len_replace >= 0);

  if (len == 0) {
    return String();
  }

  smart::vector<int> founds;
  founds.reserve(16);
  if (len_search == 1) {
    for (int pos = string_find(input, len, *search, 0, case_sensitive);
         pos >= 0;
         pos = string_find(input, len, *search, pos + len_search,
                           case_sensitive)) {
      founds.push_back(pos);
    }
  } else {
    for (int pos = string_find(input, len, search, len_search, 0,
                               case_sensitive);
         pos >= 0;
         pos = string_find(input, len, search, len_search,
                           pos + len_search, case_sensitive)) {
      founds.push_back(pos);
    }
  }

  count = founds.size();
  if (count == 0) {
    return String(); // not found
  }

  int reserve;

  // Make sure the new size of the string wouldn't overflow int32_t. Don't
  // bother if the replacement wouldn't make the string longer.
  if (len_replace > len_search) {
    auto raise = [&] { raise_error("String too large"); };
    if (mul_overflow(len_replace - len_search, count)) {
      raise();
    }
    int diff = (len_replace - len_search) * count;
    if (add_overflow(len, diff)) {
      raise();
    }
    reserve = len + diff;
  } else {
    reserve = len + (len_replace - len_search) * count;
  }

  String retString(reserve, ReserveString);
  char *ret = retString.bufferSlice().ptr;
  char *p = ret;
  int pos = 0; // last position in input that hasn't been copied over yet
  int n;
  for (unsigned int i = 0; i < founds.size(); i++) {
    n = founds[i];
    if (n > pos) {
      n -= pos;
      memcpy(p, input, n);
      p += n;
      input += n;
      pos += n;
    }
    if (len_replace) {
      memcpy(p, replacement, len_replace);
      p += len_replace;
    }
    input += len_search;
    pos += len_search;
  }
  n = len;
  if (n > pos) {
    n -= pos;
    memcpy(p, input, n);
    p += n;
  }
  retString.setSize(p - ret);
  return retString;
}