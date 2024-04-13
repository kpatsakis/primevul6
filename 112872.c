String string_uuencode(const char *src, int src_len) {
  assertx(src);
  assertx(src_len);

  int len = 45;
  char *p;
  const char *s, *e, *ee;
  char *dest;

  /* encoded length is ~ 38% greater than the original */
  String ret((int)ceil(src_len * 1.38) + 45, ReserveString);
  p = dest = ret.mutableData();
  s = src;
  e = src + src_len;

  while ((s + 3) < e) {
    ee = s + len;
    if (ee > e) {
      ee = e;
      len = ee - s;
      if (len % 3) {
        ee = s + (int) (floor(len / 3) * 3);
      }
    }
    *p++ = PHP_UU_ENC(len);

    while (s < ee) {
      *p++ = PHP_UU_ENC(*s >> 2);
      *p++ = PHP_UU_ENC_C2(s);
      *p++ = PHP_UU_ENC_C3(s);
      *p++ = PHP_UU_ENC(*(s + 2) & 077);

      s += 3;
    }

    if (len == 45) {
      *p++ = '\n';
    }
  }

  if (s < e) {
    if (len == 45) {
      *p++ = PHP_UU_ENC(e - s);
      len = 0;
    }

    *p++ = PHP_UU_ENC(*s >> 2);
    *p++ = PHP_UU_ENC_C2(s);
    *p++ = ((e - s) > 1) ? PHP_UU_ENC_C3(s) : PHP_UU_ENC('\0');
    *p++ = ((e - s) > 2) ? PHP_UU_ENC(*(s + 2) & 077) : PHP_UU_ENC('\0');
  }

  if (len < 45) {
    *p++ = '\n';
  }

  *p++ = PHP_UU_ENC('\0');
  *p++ = '\n';
  *p = '\0';

  ret.setSize(p - dest);
  return ret;
}