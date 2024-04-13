Variant HHVM_FUNCTION(mb_strimwidth,
                      const String& str,
                      int start,
                      int width,
                      const Variant& opt_trimmarker,
                      const Variant& opt_encoding) {
  const String trimmarker = convertArg(opt_trimmarker);
  const String encoding = convertArg(opt_encoding);

  mbfl_string string, result, marker, *ret;

  mbfl_string_init(&string);
  mbfl_string_init(&marker);
  string.no_language = MBSTRG(current_language);
  string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  marker.no_language = MBSTRG(current_language);
  marker.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  marker.val = nullptr;
  marker.len = 0;

  if (!encoding.empty()) {
    string.no_encoding = marker.no_encoding =
      mbfl_name2no_encoding(encoding.data());
    if (string.no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", encoding.data());
      return false;
    }
  }

  string.val = (unsigned char *)str.data();
  string.len = str.size();

  if (start < 0 || start > str.size()) {
    raise_warning("Start position is out of reange");
    return false;
  }

  if (width < 0) {
    raise_warning("Width is negative value");
    return false;
  }

  marker.val = (unsigned char *)trimmarker.data();
  marker.len = trimmarker.size();

  ret = mbfl_strimwidth(&string, &marker, &result, start, width);
  if (ret != nullptr) {
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }
  return false;
}