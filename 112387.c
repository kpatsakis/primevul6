bool HHVM_FUNCTION(key_exists,
                   const Variant& key,
                   const Variant& search) {
  return HHVM_FN(array_key_exists)(key, search);
}