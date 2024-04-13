TypedValue HHVM_FUNCTION(array_uintersect_uassoc,
                         const Variant& array1,
                         const Variant& array2,
                         const Variant& data_compare_func,
                         const Variant& key_compare_func,
                         const Array& args /* = null array */) {
  Variant data_func = data_compare_func;
  Variant key_func = key_compare_func;
  Array extra = args;
  if (!extra.empty()) {
    extra.prepend(key_func);
    extra.prepend(data_func);
    key_func = extra.pop();
    data_func = extra.pop();
  }
  diff_intersect_body(intersect, extra, true COMMA true COMMA cmp_func
                      COMMA &key_func COMMA cmp_func COMMA &data_func);
}