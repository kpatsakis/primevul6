TypedValue HHVM_FUNCTION(array_udiff,
                         const Variant& array1,
                         const Variant& array2,
                         const Variant& data_compare_func,
                         const Array& args /* = null array */) {
  Variant func = data_compare_func;
  Array extra = args;
  if (!extra.empty()) {
    extra.prepend(func);
    func = extra.pop();
  }
  diff_intersect_body(diff, extra, false COMMA true COMMA NULL COMMA NULL
                      COMMA cmp_func COMMA &func);
}