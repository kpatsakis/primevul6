Variant HHVM_FUNCTION(mb_decode_numericentity,
                      const String& str,
                      const Variant& convmap,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);
  return php_mb_numericentity_exec(str, convmap, encoding, false, 1);
}