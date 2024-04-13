bool HHVM_FUNCTION(i18n_loc_set_attribute,
                   int64_t attr,
                   int64_t val) {
  return s_collator->setAttribute(attr, val);
}