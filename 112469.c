Variant array_search(const Variant& needle,
                     const Variant& haystack,
                     bool strict /* = false */) {
  Variant ret = false;
  auto ok = strict ?
    IterateKV(*haystack.asCell(),
              [](ArrayData*) { return false; },
              [&](const TypedValue* key, const TypedValue* tv) -> bool {
                if (HPHP::same(tvAsCVarRef(tv), needle)) {
                  ret = tvAsCVarRef(key);
                  return true;
                }
                return false;
              },
              [](ObjectData*) { return false; }) :
    IterateKV(*haystack.asCell(),
              [](ArrayData*) { return false; },
              [&](const TypedValue* key, const TypedValue* tv) -> bool {
                if (HPHP::equal(tvAsCVarRef(tv), needle)) {
                  ret = tvAsCVarRef(key);
                  return true;
                }
                return false;
              },
              [](ObjectData*) { return false; });

  if (UNLIKELY(!ok)) {
    raise_warning("array_search() expects parameter 2 to be an array "
                  "or collection");
    return init_null();
  }

  return ret;
}