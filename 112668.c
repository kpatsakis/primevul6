static void php_mb_regex_set_options(OnigOptionType options,
                                     OnigSyntaxType *syntax,
                                     OnigOptionType *prev_options,
                                     OnigSyntaxType **prev_syntax) {
  if (prev_options != nullptr) {
    *prev_options = MBSTRG(regex_default_options);
  }
  if (prev_syntax != nullptr) {
    *prev_syntax = MBSTRG(regex_default_syntax);
  }
  MBSTRG(regex_default_options) = options;
  MBSTRG(regex_default_syntax) = syntax;
}