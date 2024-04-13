scan_unsigned_hexadecimal_number(UChar** src, UChar* end, int maxlen,
				 OnigEncoding enc)
{
  OnigCodePoint c;
  unsigned int num, val;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  while (! PEND && maxlen-- != 0) {
    PFETCH(c);
    if (ONIGENC_IS_CODE_XDIGIT(enc, c)) {
      val = (unsigned int )XDIGITVAL(enc,c);
      if ((INT_MAX_LIMIT - val) / 16UL < num)
        return -1;  /* overflow */

      num = (num << 4) + XDIGITVAL(enc,c);
    }
    else {
      PUNFETCH;
      break;
    }
  }
  *src = p;
  return num;
}