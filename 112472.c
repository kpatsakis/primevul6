TypedValue HHVM_FUNCTION(array_search,
                         const Variant& needle,
                         const Variant& haystack,
                         bool strict /* = false */) {
  return tvReturn(array_search(needle, haystack, strict));
}