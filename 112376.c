static void compact(PointerSet& seen,
                    VarEnv* v, Array &ret, const Variant& var) {
  if (var.isArray()) {
    auto adata = var.getArrayData();
    auto check = couldRecur(var, adata);
    if (check && !seen.insert(adata).second) {
      raise_warning("compact(): recursion detected");
      return;
    }
    for (ArrayIter iter(adata); iter; ++iter) {
      compact(seen, v, ret, iter.secondRef());
    }
    if (check) seen.erase(adata);
  } else {
    String varname = var.toString();
    if (!varname.empty() && v->lookup(varname.get()) != NULL) {
      ret.set(varname, *reinterpret_cast<Variant*>(v->lookup(varname.get())));
    }
  }
}