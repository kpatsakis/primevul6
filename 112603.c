Variant HHVM_FUNCTION(mb_ereg,
                      const Variant& pattern,
                      const String& str,
                      Variant& regs) {
  return _php_mb_regex_ereg_exec(pattern, str, &regs, 0);
}