static bool is_protected_variable(char *varname) {
  normalize_protected_variable(varname);
  auto iter = s_rfc1867_data->rfc1867ProtectedVariables.find(varname);
  return iter != s_rfc1867_data->rfc1867ProtectedVariables.end();
}