Variant HHVM_FUNCTION(mb_language,
                      const Variant& opt_language) {
  const String language = convertArg(opt_language);

  if (language.empty()) {
    return String(mbfl_no_language2name(MBSTRG(current_language)), CopyString);
  }

  mbfl_no_language no_language = mbfl_name2no_language(language.data());
  if (no_language == mbfl_no_language_invalid) {
    raise_warning("Unknown language \"%s\"", language.data());
    return false;
  }

  php_mb_nls_get_default_detect_order_list
    (no_language, &MBSTRG(default_detect_order_list),
     &MBSTRG(default_detect_order_list_size));
  MBSTRG(current_language) = no_language;
  return true;
}