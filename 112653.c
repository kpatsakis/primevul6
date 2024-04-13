Variant HHVM_FUNCTION(mb_substr,
                      const String& str,
                      int start,
                      const Variant& length /*= uninit_null() */,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);
  return php_mb_substr(str, start, length, encoding, true);
}