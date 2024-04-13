Variant HHVM_FUNCTION(mb_strripos,
                      const String& haystack,
                      const String& needle,
                      int offset /* = 0 */,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);

  const char *from_encoding;
  if (encoding.empty()) {
    from_encoding = MBSTRG(current_internal_encoding)->mime_name;
  } else {
    from_encoding = encoding.data();
  }

  int n = php_mb_stripos(1, haystack.data(), haystack.size(),
                         needle.data(), needle.size(), offset, from_encoding);
  if (n >= 0) {
    return n;
  }
  return false;
}