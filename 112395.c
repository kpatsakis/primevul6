bool HHVM_FUNCTION(in_array,
                   const Variant& needle,
                   const Variant& haystack,
                   bool strict /* = false */) {
  bool ret = false;
  auto ok = strict ?
    IterateV(*haystack.asCell(),
             [](ArrayData*) { return false; },
             [&](const TypedValue* tv) -> bool {
               if (HPHP::same(tvAsCVarRef(tv), needle)) {
                 ret = true;
                 return true;
               }
               return false;
             },
             [](ObjectData*) { return false; }) :
    IterateV(*haystack.asCell(),
             [](ArrayData*) { return false; },
             [&](const TypedValue* tv) -> bool {
               if (HPHP::equal(tvAsCVarRef(tv), needle)) {
                 ret = true;
                 return true;
               }
               return false;
             },
             [](ObjectData*) { return false; });

  if (UNLIKELY(!ok)) {
    raise_warning("in_array() expects parameter 2 to be an array "
                  "or collection");
  }
  return ret;
}