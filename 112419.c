Array HHVM_FUNCTION(compact,
                    const Variant& varname,
                    const Array& args /* = null array */) {
  raise_disallowed_dynamic_call("compact should not be called dynamically");
  Array ret = Array::attach(PackedArray::MakeReserve(args.size() + 1));
  VarEnv* v = g_context->getOrCreateVarEnv();
  if (v) {
    PointerSet seen;
    compact(seen, v, ret, varname);
    if (!args.empty()) compact(seen, v, ret, args);
  }
  return ret;
}