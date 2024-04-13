String HHVM_FUNCTION(i18n_loc_get_default) {
  return s_collator->getLocale();
}