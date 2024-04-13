bool HHVM_FUNCTION(array_walk_recursive,
                   VRefParam input,
                   const Variant& funcname,
                   const Variant& userdata /* = null_variant */) {
  if (!input.isArray()) {
    throw_expected_array_exception("array_walk_recursive");
    return false;
  }
  CallCtx ctx;
  CallerFrame cf;
  vm_decode_function(funcname, cf(), false, ctx);
  if (ctx.func == NULL) {
    return false;
  }
  PointerSet seen;
  Variant var(input, Variant::WithRefBind{});
  ArrayUtil::Walk(var, walk_func, &ctx, true, &seen, userdata);
  return true;
}