static void safe_php_register_variable(char *var, const Variant& val,
                                       Array& track_vars_array,
                                       bool override_protection) {
  if (override_protection || !is_protected_variable(var)) {
    register_variable(track_vars_array, var, val);
  }
}