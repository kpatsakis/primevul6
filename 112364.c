Variant HHVM_FUNCTION(array_keys, int64_t argc,
                                  const Variant& input,
                                  const Variant& search_value /*=null*/,
                                  bool strict /*=false*/) {
  return array_keys_helper(
    input,
    argc < 2 ? null_variant : search_value,
    strict
  );
}