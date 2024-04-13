Variant HHVM_FUNCTION(mb_decode_mimeheader,
                      const String& str) {
  mbfl_string string, result, *ret;

  mbfl_string_init(&string);
  string.no_language = MBSTRG(current_language);
  string.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  string.val = (unsigned char *)str.data();
  string.len = str.size();

  mbfl_string_init(&result);
  ret = mbfl_mime_header_decode(&string, &result,
                                MBSTRG(current_internal_encoding)->no_encoding);
  if (ret != nullptr) {
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }
  return false;
}