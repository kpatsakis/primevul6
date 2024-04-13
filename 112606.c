Variant HHVM_FUNCTION(mb_strlen,
                      const String& str,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);

  mbfl_string string;
  mbfl_string_init(&string);
  string.val = (unsigned char *)str.data();
  string.len = str.size();
  string.no_language = MBSTRG(current_language);

  if (encoding.empty()) {
    string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  } else {
    string.no_encoding = mbfl_name2no_encoding(encoding.data());
    if (string.no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", encoding.data());
      return false;
    }
  }

  int n = mbfl_strlen(&string);
  if (n >= 0) {
    return n;
  }
  return false;
}