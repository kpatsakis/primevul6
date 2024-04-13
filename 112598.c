Variant HHVM_FUNCTION(mb_strtoupper,
                      const String& str,
                      const Variant& opt_encoding) {
  /* Fast-case for empty static string without dereferencing any pointers. */
  if (str.get() == staticEmptyString()) return empty_string_variant();
  if (LIKELY(isUtf8(opt_encoding))) {
    /* Fast-case for ASCII. */
    if (auto sd = str.get()) {
      auto sl = sd->slice();
      auto r = isUtf8AsciiUpper(sl);
      if (r > 0) {
        return str;
      } else if (r < 0) {
        return String::attach(asciiToUpper(sd));
      }
    }
  }
  const String encoding = convertArg(opt_encoding);

  const char *from_encoding;
  if (encoding.empty()) {
    from_encoding = MBSTRG(current_internal_encoding)->mime_name;
  } else {
    from_encoding = encoding.data();
  }

  unsigned int ret_len;
  char *newstr = php_unicode_convert_case(PHP_UNICODE_CASE_UPPER,
                                          str.data(), str.size(),
                                          &ret_len, from_encoding);
  if (newstr) {
    return String(newstr, ret_len, AttachString);
  }
  return false;
}