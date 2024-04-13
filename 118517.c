hermesBuiltinEnsureObject(void *, Runtime *runtime, NativeArgs args) {
  if (LLVM_LIKELY(args.getArg(0).isObject()))
    return HermesValue::encodeUndefinedValue();

  return runtime->raiseTypeError(args.getArgHandle(1));
}