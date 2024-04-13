static Variant php_mb_substr(const String& str, int from,
                             const Variant& vlen,
                             const String& encoding, bool substr) {
  mbfl_string string;
  mbfl_string_init(&string);
  string.no_language = MBSTRG(current_language);
  string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  string.val = (unsigned char *)str.data();
  string.len = str.size();

  if (!encoding.empty()) {
    string.no_encoding = mbfl_name2no_encoding(encoding.data());
    if (string.no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", encoding.data());
      return false;
    }
  }

  int len = vlen.toInt64();
  int size = 0;

  if (substr) {
    int size_tmp = -1;
    if (vlen.isNull() || len == 0x7FFFFFFF) {
      size_tmp = mbfl_strlen(&string);
      len = size_tmp;
    }
    if (from < 0 || len < 0) {
      size = size_tmp < 0 ? mbfl_strlen(&string) : size_tmp;
    }
  } else {
    size = str.size();
    if (vlen.isNull() || len == 0x7FFFFFFF) {
      len = size;
    }
  }

  /* if "from" position is negative, count start position from the end
   * of the string
   */
  if (from < 0) {
    from = size + from;
    if (from < 0) {
      from = 0;
    }
  }

  /* if "length" position is negative, set it to the length
   * needed to stop that many chars from the end of the string
   */
  if (len < 0) {
    len = (size - from) + len;
    if (len < 0) {
      len = 0;
    }
  }

  if (!substr && from > size) {
    return false;
  }

  mbfl_string result;
  mbfl_string *ret;
  if (substr) {
    ret = mbfl_substr(&string, &result, from, len);
  } else {
    ret = mbfl_strcut(&string, &result, from, len);
  }
  if (ret != nullptr) {
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }
  return false;
}