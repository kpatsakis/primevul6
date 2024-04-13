TypedValue HHVM_FUNCTION(array_intersect_assoc,
                         const Variant& array1,
                         const Variant& array2,
                         const Array& args /* = null array */) {
  diff_intersect_body(intersect, args, true COMMA true);
}