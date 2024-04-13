Variant HHVM_FUNCTION(mb_ereg_replace,
                      const Variant& pattern,
                      const String& replacement,
                      const String& str,
                      const Variant& opt_option) {
  const String option = convertArg(opt_option);
  return _php_mb_regex_ereg_replace_exec(pattern, replacement,
                                         str, option, 0);
}