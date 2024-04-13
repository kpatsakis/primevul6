Variant HHVM_FUNCTION(mb_ereg_search_pos,
                      const Variant& opt_pattern,
                      const Variant& opt_option) {
  const String pattern = convertArg(opt_pattern);
  const String option = convertArg(opt_option);
  return _php_mb_regex_ereg_search_exec(pattern, option, 1);
}