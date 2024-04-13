static int php_mb_stripos(int mode,
                          const char *old_haystack, int old_haystack_len,
                          const char *old_needle, int old_needle_len,
                          long offset, const char *from_encoding) {
  int n;
  mbfl_string haystack, needle;
  n = -1;

  mbfl_string_init(&haystack);
  mbfl_string_init(&needle);
  haystack.no_language = MBSTRG(current_language);
  haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  needle.no_language = MBSTRG(current_language);
  needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;

  do {
    haystack.val = (unsigned char *)php_unicode_convert_case
      (PHP_UNICODE_CASE_UPPER, old_haystack, (size_t)old_haystack_len,
       &haystack.len, from_encoding);
    if (!haystack.val) {
      break;
    }
    if (haystack.len <= 0) {
      break;
    }

    needle.val = (unsigned char *)php_unicode_convert_case
      (PHP_UNICODE_CASE_UPPER, old_needle, (size_t)old_needle_len,
       &needle.len, from_encoding);
    if (!needle.val) {
      break;
    }
    if (needle.len <= 0) {
      break;
    }

    haystack.no_encoding = needle.no_encoding =
      mbfl_name2no_encoding(from_encoding);
    if (haystack.no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", from_encoding);
      break;
    }

    int haystack_char_len = mbfl_strlen(&haystack);
    if (mode) {
      if ((offset > 0 && offset > haystack_char_len) ||
          (offset < 0 && -offset > haystack_char_len)) {
        raise_warning("Offset is greater than the length of haystack string");
        break;
      }
    } else {
      if (offset < 0 || offset > haystack_char_len) {
        raise_warning("Offset not contained in string.");
        break;
      }
    }

    n = mbfl_strpos(&haystack, &needle, offset, mode);
  } while(0);

  if (haystack.val) {
    free(haystack.val);
  }
  if (needle.val) {
    free(needle.val);
  }
  return n;
}