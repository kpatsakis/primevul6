Array HHVM_FUNCTION(__SystemLib_compact_sl,
                    const Variant& varname,
                    const Array& args /* = null array */) {
  Array ret = Array::attach(PackedArray::MakeReserve(args.size() + 1));
  VarEnv* v = g_context->getOrCreateVarEnv();
  if (v) {
    PointerSet seen;
    compact(seen, v, ret, varname);
    if (!args.empty()) compact(seen, v, ret, args);
  }
  return ret;
}