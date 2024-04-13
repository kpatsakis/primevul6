static TypedValue* HHVM_FN(wddx_serialize_vars)(ActRec* ar) {
  return serialize_vars_helper(ar);
}