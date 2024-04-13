static int cmp_func(const Variant& v1, const Variant& v2, const void *data) {
  Variant *callback = (Variant *)data;
  return vm_call_user_func(*callback, make_packed_array(v1, v2)).toInt32();
}