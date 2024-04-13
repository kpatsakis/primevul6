static Variant _php_mb_regex_ereg_search_exec(const String& pattern,
                                              const String& option,
                                              int mode) {
  int n, i, err, pos, len, beg, end;
  OnigUChar *str;
  OnigSyntaxType *syntax = MBSTRG(regex_default_syntax);
  OnigOptionType noption;

  noption = MBSTRG(regex_default_options);
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

  pos = MBSTRG(search_pos);
  str = nullptr;
  len = 0;
  if (!MBSTRG(search_str).empty()) {
    str = (OnigUChar *)MBSTRG(search_str).data();
    len = MBSTRG(search_str).size();
  }

  if (MBSTRG(search_re) == nullptr) {
    raise_warning("No regex given");
    return false;
  }

  if (str == nullptr) {
    raise_warning("No string given");
    return false;
  }

  if (MBSTRG(search_regs)) {
    onig_region_free(MBSTRG(search_regs), 1);
  }
  MBSTRG(search_regs) = onig_region_new();

  err = onig_search(MBSTRG(search_re), str, str + len, str + pos, str  + len,
                    MBSTRG(search_regs), 0);
  Variant ret;
  if (err == ONIG_MISMATCH) {
    MBSTRG(search_pos) = len;
    ret = false;
  } else if (err <= -2) {
    OnigUChar err_str[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(err_str, err);
    raise_warning("mbregex search failure in mbregex_search(): %s", err_str);
    ret = false;
  } else {
    if (MBSTRG(search_regs)->beg[0] == MBSTRG(search_regs)->end[0]) {
      raise_warning("Empty regular expression");
    }
    switch (mode) {
    case 1:
      {
        beg = MBSTRG(search_regs)->beg[0];
        end = MBSTRG(search_regs)->end[0];
        ret = make_packed_array(beg, end - beg);
      }
      break;
    case 2:
      n = MBSTRG(search_regs)->num_regs;
      ret = Variant(Array::Create());
      for (i = 0; i < n; i++) {
        beg = MBSTRG(search_regs)->beg[i];
        end = MBSTRG(search_regs)->end[i];
        if (beg >= 0 && beg <= end && end <= len) {
          ret.toArrRef().append(
            String((const char *)(str + beg), end - beg, CopyString));
        } else {
          ret.toArrRef().append(false);
        }
      }
      break;
    default:
      ret = true;
      break;
    }
    end = MBSTRG(search_regs)->end[0];
    if (pos < end) {
      MBSTRG(search_pos) = end;
    } else {
      MBSTRG(search_pos) = pos + 1;
    }
  }

  if (err < 0) {
    onig_region_free(MBSTRG(search_regs), 1);
    MBSTRG(search_regs) = (OnigRegion *)nullptr;
  }
  return ret;
}