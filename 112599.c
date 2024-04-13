bool HHVM_FUNCTION(mb_ereg_search_init,
                   const String& str,
                   const Variant& opt_pattern,
                   const Variant& opt_option) {
  const String pattern = convertArg(opt_pattern);
  const String option = convertArg(opt_option);

  OnigOptionType noption = MBSTRG(regex_default_options);
  OnigSyntaxType *syntax = MBSTRG(regex_default_syntax);
  if (!option.empty()) {
    noption = 0;
    _php_mb_regex_init_options(option.data(), option.size(),
                               &noption, &syntax, nullptr);
  }
  if (!pattern.empty()) {
    if ((MBSTRG(search_re) = php_mbregex_compile_pattern
         (pattern, noption, MBSTRG(current_mbctype), syntax)) == nullptr) {
      return false;
    }
  }

  MBSTRG(search_str) = std::string(str.data(), str.size());
  MBSTRG(search_pos) = 0;

  if (MBSTRG(search_regs) != nullptr) {
    onig_region_free(MBSTRG(search_regs), 1);
    MBSTRG(search_regs) = (OnigRegion *)nullptr;
  }
  return true;
}