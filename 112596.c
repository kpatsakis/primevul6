static unsigned long php_unicode_toupper(unsigned long code,
                                         enum mbfl_no_encoding enc) {
  int field;
  long l, r;

  if (php_unicode_is_upper(code))
    return code;

  if (php_unicode_is_lower(code)) {
    /*
     * The character is lower case.
     */
    field = 2;
    l = _uccase_len[0];
    r = (l + _uccase_len[1]) - 3;

    if (enc == mbfl_no_encoding_8859_9) {
      return php_turkish_toupper(code, l, r, field);
    }

  } else {
    /*
     * The character is title case.
     */
    field = 1;
    l = _uccase_len[0] + _uccase_len[1];
    r = _uccase_size - 3;
  }
  return case_lookup(code, l, r, field);
}