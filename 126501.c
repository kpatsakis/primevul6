static bool mg_aton6(struct mg_str str, struct mg_addr *addr) {
  size_t i, j = 0, n = 0, dc = 42;
  if (str.len > 2 && str.ptr[0] == '[') str.ptr++, str.len -= 2;
  if (mg_v4mapped(str, addr)) return true;
  for (i = 0; i < str.len; i++) {
    if ((str.ptr[i] >= '0' && str.ptr[i] <= '9') ||
        (str.ptr[i] >= 'a' && str.ptr[i] <= 'f') ||
        (str.ptr[i] >= 'A' && str.ptr[i] <= 'F')) {
      unsigned long val;
      if (i > j + 3) return false;
      // LOG(LL_DEBUG, ("%zu %zu [%.*s]", i, j, (int) (i - j + 1),
      // &str.ptr[j]));
      val = mg_unhexn(&str.ptr[j], i - j + 1);
      addr->ip6[n] = (uint8_t) ((val >> 8) & 255);
      addr->ip6[n + 1] = (uint8_t) (val & 255);
    } else if (str.ptr[i] == ':') {
      j = i + 1;
      if (i > 0 && str.ptr[i - 1] == ':') {
        dc = n;  // Double colon
        if (i > 1 && str.ptr[i - 2] == ':') return false;
      } else if (i > 0) {
        n += 2;
      }
      if (n > 14) return false;
      addr->ip6[n] = addr->ip6[n + 1] = 0;  // For trailing ::
    } else {
      return false;
    }
  }
  if (n < 14 && dc == 42) return false;
  if (n < 14) {
    memmove(&addr->ip6[dc + (14 - n)], &addr->ip6[dc], n - dc + 2);
    memset(&addr->ip6[dc], 0, 14 - n);
  }
  addr->is_ip6 = true;
  return true;
}