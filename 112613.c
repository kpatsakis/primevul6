Variant HHVM_FUNCTION(mb_strrichr,
                      const String& haystack,
                      const String& needle,
                      bool part /* = false */,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);

  mbfl_string mbs_haystack;
  mbfl_string_init(&mbs_haystack);
  mbs_haystack.no_language = MBSTRG(current_language);
  mbs_haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  mbs_haystack.val = (unsigned char *)haystack.data();
  mbs_haystack.len = haystack.size();

  mbfl_string mbs_needle;
  mbfl_string_init(&mbs_needle);
  mbs_needle.no_language = MBSTRG(current_language);
  mbs_needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  mbs_needle.val = (unsigned char *)needle.data();
  mbs_needle.len = needle.size();

  const char *from_encoding;
  if (encoding.empty()) {
    from_encoding = MBSTRG(current_internal_encoding)->mime_name;
  } else {
    from_encoding = encoding.data();
  }
  mbs_haystack.no_encoding = mbs_needle.no_encoding =
    mbfl_name2no_encoding(from_encoding);
  if (mbs_haystack.no_encoding == mbfl_no_encoding_invalid) {
    raise_warning("Unknown encoding \"%s\"", from_encoding);
    return false;
  }

  int n = php_mb_stripos(1, (const char*)mbs_haystack.val, mbs_haystack.len,
                         (const char*)mbs_needle.val, mbs_needle.len,
                         0, from_encoding);
  if (n < 0) {
    return false;
  }

  mbfl_string result, *ret = nullptr;
  int mblen = mbfl_strlen(&mbs_haystack);
  if (part) {
    ret = mbfl_substr(&mbs_haystack, &result, 0, n);
  } else {
    int len = (mblen - n);
    ret = mbfl_substr(&mbs_haystack, &result, n, len);
  }

  if (ret != nullptr) {
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }
  return false;
}