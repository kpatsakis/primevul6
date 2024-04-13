static void walk_func(Variant& value,
                      const Variant& key,
                      const Variant& userdata,
                      const void *data) {
  CallCtx* ctx = (CallCtx*)data;
  Variant sink;
  int nargs = userdata.isInitialized() ? 3 : 2;
  TypedValue args[3] = { *value.asRef(), *key.asCell(), *userdata.asCell() };
  g_context->invokeFuncFew(sink.asTypedValue(), *ctx, nargs, args);
}