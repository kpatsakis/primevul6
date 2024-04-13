static unsigned long php_unicode_tolower(unsigned long code,
                                         enum mbfl_no_encoding enc) {
  int field;
  long l, r;

  if (php_unicode_is_lower(code))
    return code;

  if (php_unicode_is_upper(code)) {
    /*
     * The character is upper case.
     */
    field = 1;
    l = 0;
    r = _uccase_len[0] - 3;

    if (enc == mbfl_no_encoding_8859_9) {
      return php_turkish_tolower(code, l, r, field);
    }

  } else {
    /*
     * The character is title case.
     */
    field = 2;
    l = _uccase_len[0] + _uccase_len[1];
    r = _uccase_size - 3;
  }
  return case_lookup(code, l, r, field);
}