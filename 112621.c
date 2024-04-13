static php_mb_regex_t *php_mbregex_compile_pattern(const String& pattern,
                                                   OnigOptionType options,
                                                   OnigEncoding enc,
                                                   OnigSyntaxType *syntax) {
  int err_code = 0;
  OnigErrorInfo err_info;
  OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
  php_mb_regex_t *rc = nullptr;

  std::string spattern = std::string(pattern.data(), pattern.size());
  RegexCache &cache = MBSTRG(ht_rc);
  RegexCache::const_iterator it =
    cache.find(spattern);
  if (it != cache.end()) {
    rc = it->second;
  }

  if (!rc || onig_get_options(rc) != options || onig_get_encoding(rc) != enc ||
      onig_get_syntax(rc) != syntax) {
    if (rc) {
      onig_free(rc);
      rc = nullptr;
    }
    if ((err_code = onig_new(&rc, (OnigUChar *)pattern.data(),
                             (OnigUChar *)(pattern.data() + pattern.size()),
                             options,enc, syntax, &err_info)) != ONIG_NORMAL) {
      onig_error_code_to_str(err_str, err_code, err_info);
      raise_warning("mbregex compile err: %s", err_str);
      return nullptr;
    }
    MBSTRG(ht_rc)[spattern] = rc;
  }
  return rc;
}