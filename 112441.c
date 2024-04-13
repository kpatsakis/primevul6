TypedValue HHVM_FUNCTION(array_intersect_uassoc,
                         const Variant& array1,
                         const Variant& array2,
                         const Variant& key_compare_func,
                         const Array& args /* = null array */) {
  Variant func = key_compare_func;
  Array extra = args;
  if (!extra.empty()) {
    extra.prepend(func);
    func = extra.pop();
  }
  diff_intersect_body(intersect, extra, true COMMA true
                      COMMA cmp_func COMMA &func);
}