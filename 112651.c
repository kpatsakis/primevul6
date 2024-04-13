Variant HHVM_FUNCTION(mb_split,
                      const String& pattern,
                      const String& str,
                      int count /* = -1 */) {
  php_mb_regex_t *re;
  OnigRegion *regs = nullptr;

  int n, err;
  if (count == 0) {
    count = 1;
  }

  /* create regex pattern buffer */
  if ((re = php_mbregex_compile_pattern(pattern,
                                        MBSTRG(regex_default_options),
                                        MBSTRG(current_mbctype),
                                        MBSTRG(regex_default_syntax)))
      == nullptr) {
    return false;
  }

  Array ret;
  OnigUChar *pos0 = (OnigUChar *)str.data();
  OnigUChar *pos_end = (OnigUChar *)(str.data() + str.size());
  OnigUChar *pos = pos0;
  err = 0;
  regs = onig_region_new();
  /* churn through str, generating array entries as we go */
  while ((--count != 0) &&
         (err = onig_search(re, pos0, pos_end, pos, pos_end, regs, 0)) >= 0) {
    if (regs->beg[0] == regs->end[0]) {
      raise_warning("Empty regular expression");
      break;
    }

    /* add it to the array */
    if (regs->beg[0] < str.size() && regs->beg[0] >= (pos - pos0)) {
      ret.append(String((const char *)pos,
                        ((OnigUChar *)(str.data() + regs->beg[0]) - pos),
                        CopyString));
    } else {
      err = -2;
      break;
    }
    /* point at our new starting point */
    n = regs->end[0];
    if ((pos - pos0) < n) {
      pos = pos0 + n;
    }
    if (count < 0) {
      count = 0;
    }
    onig_region_free(regs, 0);
  }

  onig_region_free(regs, 1);

  /* see if we encountered an error */
  if (err <= -2) {
    OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(err_str, err);
    raise_warning("mbregex search failure in mbsplit(): %s", err_str);
    return false;
  }

  /* otherwise we just have one last element to add to the array */
  n = pos_end - pos;
  if (n > 0) {
    ret.append(String((const char *)pos, n, CopyString));
  } else {
    ret.append("");
  }
  return ret;
}