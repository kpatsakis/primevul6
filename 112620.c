Variant HHVM_FUNCTION(mb_stripos,
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

  if (needle.empty()) {
    raise_warning("Empty delimiter");
    return false;
  }

  int n = php_mb_stripos(0, haystack.data(), haystack.size(),
                         needle.data(), needle.size(), offset, from_encoding);
  if (n >= 0) {
    return n;
  }
  return false;
}