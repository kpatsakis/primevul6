hermesBuiltinThrowTypeError(void *, Runtime *runtime, NativeArgs args) {
  return runtime->raiseTypeError(args.getArgHandle(0));
}