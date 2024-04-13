String string_number_format(double d, int dec,
                            const String& dec_point,
                            const String& thousand_sep) {
  char *tmpbuf = nullptr, *resbuf;
  char *s, *t;  /* source, target */
  char *dp;
  int integral;
  int tmplen, reslen=0;
  int count=0;
  int is_negative=0;

  if (d < 0) {
    is_negative = 1;
    d = -d;
  }

  if (dec < 0) dec = 0;
  d = php_math_round(d, dec);

  // departure from PHP: we got rid of dependencies on spprintf() here.
  String tmpstr(63, ReserveString);
  tmpbuf = tmpstr.bufferSlice().ptr;
  snprintf(tmpbuf, 64, "%.*F", dec, d);
  tmplen = strlen(tmpbuf);
  if (tmpbuf == nullptr || !isdigit((int)tmpbuf[0])) {
    tmpstr.setSize(tmplen);
    return tmpstr;
  }

  /* find decimal point, if expected */
  if (dec) {
    dp = strpbrk(tmpbuf, ".,");
  } else {
    dp = nullptr;
  }

  /* calculate the length of the return buffer */
  if (dp) {
    integral = dp - tmpbuf;
  } else {
    /* no decimal point was found */
    integral = tmplen;
  }

  /* allow for thousand separators */
  if (!thousand_sep.empty()) {
    integral += ((integral-1) / 3) * thousand_sep.size();
  }

  reslen = integral;

  if (dec) {
    reslen += dec;

    if (!dec_point.empty()) {
      reslen += dec_point.size();
    }
  }

  /* add a byte for minus sign */
  if (is_negative) {
    reslen++;
  }
  String resstr(reslen, ReserveString);
  resbuf = resstr.bufferSlice().ptr;

  s = tmpbuf+tmplen-1;
  t = resbuf+reslen-1;

  /* copy the decimal places.
   * Take care, as the sprintf implementation may return less places than
   * we requested due to internal buffer limitations */
  if (dec) {
    int declen = dp ? s - dp : 0;
    int topad = dec > declen ? dec - declen : 0;

    /* pad with '0's */
    while (topad--) {
      *t-- = '0';
    }

    if (dp) {
      s -= declen + 1; /* +1 to skip the point */
      t -= declen;

      /* now copy the chars after the point */
      memcpy(t + 1, dp + 1, declen);
    }

    /* add decimal point */
    if (!dec_point.empty()) {
      memcpy(t + (1 - dec_point.size()), dec_point.data(), dec_point.size());
      t -= dec_point.size();
    }
  }

  /* copy the numbers before the decimal point, adding thousand
   * separator every three digits */
  while(s >= tmpbuf) {
    *t-- = *s--;
    if (thousand_sep && (++count%3)==0 && s>=tmpbuf) {
      memcpy(t + (1 - thousand_sep.size()),
             thousand_sep.data(),
             thousand_sep.size());
      t -= thousand_sep.size();
    }
  }

  /* and a minus sign, if needed */
  if (is_negative) {
    *t-- = '-';
  }

  resstr.setSize(reslen);
  return resstr;
}