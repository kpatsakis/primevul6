String string_uudecode(const char *src, int src_len) {
  int total_len = 0;
  int len;
  const char *s, *e, *ee;
  char *p, *dest;

  String ret(ceil(src_len * 0.75), ReserveString);
  p = dest = ret.bufferSlice().ptr;
  s = src;
  e = src + src_len;

  while (s < e) {
    if ((len = PHP_UU_DEC(*s++)) <= 0) {
      break;
    }
    /* sanity check */
    if (len > src_len) {
      goto err;
    }

    total_len += len;

    ee = s + (len == 45 ? 60 : (int) floor(len * 1.33));
    /* sanity check */
    if (ee > e) {
      goto err;
    }

    while (s < ee) {
      if (s + 4 > e) goto err;

      *p++ = PHP_UU_DEC(*s) << 2 | PHP_UU_DEC(*(s + 1)) >> 4;
      *p++ = PHP_UU_DEC(*(s + 1)) << 4 | PHP_UU_DEC(*(s + 2)) >> 2;
      *p++ = PHP_UU_DEC(*(s + 2)) << 6 | PHP_UU_DEC(*(s + 3));
      s += 4;
    }

    if (len < 45) {
      break;
    }

    /* skip \n */
    s++;
  }

  if ((len = total_len > (p - dest))) {
    *p++ = PHP_UU_DEC(*s) << 2 | PHP_UU_DEC(*(s + 1)) >> 4;
    if (len > 1) {
      *p++ = PHP_UU_DEC(*(s + 1)) << 4 | PHP_UU_DEC(*(s + 2)) >> 2;
      if (len > 2) {
        *p++ = PHP_UU_DEC(*(s + 2)) << 6 | PHP_UU_DEC(*(s + 3));
      }
    }
  }

  ret.setSize(total_len);
  return ret;

 err:
  return String();
}