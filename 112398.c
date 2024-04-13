bool HHVM_FUNCTION(i18n_loc_set_strength,
                   int64_t strength) {
  return s_collator->setStrength(strength);
}