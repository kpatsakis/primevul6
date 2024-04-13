php_unicode_totitle(unsigned long code, enum mbfl_no_encoding /*enc*/) {
  int field;
  long l, r;

  if (php_unicode_is_title(code))
    return code;

  /*
   * The offset will always be the same for converting to title case.
   */
  field = 2;

  if (php_unicode_is_upper(code)) {
    /*
     * The character is upper case.
     */
    l = 0;
    r = _uccase_len[0] - 3;
  } else {
    /*
     * The character is lower case.
     */
    l = _uccase_len[0];
    r = (l + _uccase_len[1]) - 3;
  }
  return case_lookup(code, l, r, field);

}