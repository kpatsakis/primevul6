Variant HHVM_FUNCTION(mb_ereg_search_getregs) {
  OnigRegion *search_regs = MBSTRG(search_regs);
  if (search_regs && !MBSTRG(search_str).empty()) {
    Array ret;
    OnigUChar *str = (OnigUChar *)MBSTRG(search_str).data();
    int len = MBSTRG(search_str).size();
    int n = search_regs->num_regs;
    for (int i = 0; i < n; i++) {
      int beg = search_regs->beg[i];
      int end = search_regs->end[i];
      if (beg >= 0 && beg <= end && end <= len) {
        ret.append(String((const char *)(str + beg), end - beg, CopyString));
      } else {
        ret.append(false);
      }
    }
    return ret;
  }
  return false;
}