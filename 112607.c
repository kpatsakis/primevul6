Variant HHVM_FUNCTION(mb_convert_case,
                      const String& str,
                      int mode,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);

  const char *enc = nullptr;
  if (encoding.empty()) {
    enc = MBSTRG(current_internal_encoding)->mime_name;
  } else {
    enc = encoding.data();
  }

  unsigned int ret_len;
  char *newstr = php_unicode_convert_case(mode, str.data(), str.size(),
                                          &ret_len, enc);
  if (newstr) {
    return String(newstr, ret_len, AttachString);
  }
  return false;
}