static void add_protected_variable(char *varname) {
  normalize_protected_variable(varname);
  s_rfc1867_data->rfc1867ProtectedVariables.insert(varname);
}