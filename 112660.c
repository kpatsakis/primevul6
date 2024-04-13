static Variant _php_mb_regex_ereg_exec(const Variant& pattern, const String& str,
                                       Variant *regs, int icase) {
  php_mb_regex_t *re;
  OnigRegion *regions = nullptr;
  int i, match_len, beg, end;
  OnigOptionType options;

  options = MBSTRG(regex_default_options);
  if (icase) {
    options |= ONIG_OPTION_IGNORECASE;
  }

  /* compile the regular expression from the supplied regex */
  String spattern;
  if (!pattern.isString()) {
    /* we convert numbers to integers and treat them as a string */
    if (pattern.is(KindOfDouble)) {
      spattern = String(pattern.toInt64()); /* get rid of decimal places */
    } else {
      spattern = pattern.toString();
    }
  } else {
    spattern = pattern.toString();
  }
  re = php_mbregex_compile_pattern(spattern, options, MBSTRG(current_mbctype),
                                   MBSTRG(regex_default_syntax));
  if (re == nullptr) {
    return false;
  }

  regions = onig_region_new();

  /* actually execute the regular expression */
  if (onig_search(re, (OnigUChar *)str.data(),
                  (OnigUChar *)(str.data() + str.size()),
                  (OnigUChar *)str.data(),
                  (OnigUChar *)(str.data() + str.size()),
                  regions, 0) < 0) {
    onig_region_free(regions, 1);
    return false;
  }

  const char *s = str.data();
  int string_len = str.size();
  match_len = regions->end[0] - regions->beg[0];

  PackedArrayInit regsPai(regions->num_regs);
  for (i = 0; i < regions->num_regs; i++) {
    beg = regions->beg[i];
    end = regions->end[i];
    if (beg >= 0 && beg < end && end <= string_len) {
      regsPai.append(String(s + beg, end - beg, CopyString));
    } else {
      regsPai.append(false);
    }
  }
  if (regs) *regs = regsPai.toArray();

  if (match_len == 0) {
    match_len = 1;
  }
  if (regions != nullptr) {
    onig_region_free(regions, 1);
  }
  return match_len;
}