bool HHVM_FUNCTION(mb_ereg_match,
                   const String& pattern,
                   const String& str,
                   const Variant& opt_option) {
  const String option = convertArg(opt_option);

  OnigSyntaxType *syntax;
  OnigOptionType noption = 0;
  if (!option.empty()) {
    _php_mb_regex_init_options(option.data(), option.size(), &noption,
                               &syntax, nullptr);
  } else {
    noption |= MBSTRG(regex_default_options);
    syntax = MBSTRG(regex_default_syntax);
  }

  php_mb_regex_t *re;
  if ((re = php_mbregex_compile_pattern
       (pattern, noption, MBSTRG(current_mbctype), syntax)) == nullptr) {
    return false;
  }

  /* match */
  int err = onig_match(re, (OnigUChar *)str.data(),
                       (OnigUChar *)(str.data() + str.size()),
                       (OnigUChar *)str.data(), nullptr, 0);
  return err >= 0;
}