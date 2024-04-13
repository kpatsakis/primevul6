hermesBuiltinGeneratorSetDelegated(void *, Runtime *runtime, NativeArgs args) {
  auto *gen = dyn_vmcast_or_null<GeneratorInnerFunction>(
      runtime->getCurrentFrame().getPreviousFrame().getCalleeClosure());
  if (!gen) {
    return runtime->raiseTypeError(
        "generatorSetDelegated can only be called as part of yield*");
  }
  gen->setIsDelegated(true);
  return HermesValue::encodeUndefinedValue();
}