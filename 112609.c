String HHVM_FUNCTION(mb_regex_set_options,
                     const Variant& opt_options) {
  const String options = convertArg(opt_options);

  OnigOptionType opt;
  OnigSyntaxType *syntax;
  char buf[16];

  if (!options.empty()) {
    opt = 0;
    syntax = nullptr;
    _php_mb_regex_init_options(options.data(), options.size(),
                               &opt, &syntax, nullptr);
    php_mb_regex_set_options(opt, syntax, nullptr, nullptr);
  } else {
    opt = MBSTRG(regex_default_options);
    syntax = MBSTRG(regex_default_syntax);
  }
  _php_mb_regex_get_option_string(buf, sizeof(buf), opt, syntax);
  return String(buf, CopyString);
}