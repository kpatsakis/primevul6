bool HHVM_FUNCTION(i18n_loc_set_default,
                   const String& locale) {
  return s_collator->setLocale(locale);
}