hermesBuiltinExportAll(void *, Runtime *runtime, NativeArgs args) {
  Handle<JSObject> exports = args.dyncastArg<JSObject>(0);
  if (LLVM_UNLIKELY(!exports)) {
    return runtime->raiseTypeError(
        "exportAll() exports argument must be object");
  }

  Handle<JSObject> source = args.dyncastArg<JSObject>(1);
  if (LLVM_UNLIKELY(!source)) {
    return runtime->raiseTypeError(
        "exportAll() source argument must be object");
  }

  MutableHandle<> propertyHandle{runtime};

  auto dpf = DefinePropertyFlags::getDefaultNewPropertyFlags();
  dpf.configurable = 0;

  CallResult<bool> defineRes{ExecutionStatus::EXCEPTION};

  // Iterate the named properties excluding those which use Symbols.
  bool result = HiddenClass::forEachPropertyWhile(
      runtime->makeHandle(source->getClass(runtime)),
      runtime,
      [&source, &exports, &propertyHandle, &dpf, &defineRes](
          Runtime *runtime, SymbolID id, NamedPropertyDescriptor desc) {
        if (!desc.flags.enumerable)
          return true;

        if (id == Predefined::getSymbolID(Predefined::defaultExport)) {
          return true;
        }

        propertyHandle = JSObject::getNamedSlotValue(*source, runtime, desc);
        defineRes = JSObject::defineOwnProperty(
            exports, runtime, id, dpf, propertyHandle);
        if (LLVM_UNLIKELY(defineRes == ExecutionStatus::EXCEPTION)) {
          return false;
        }

        return true;
      });
  if (LLVM_UNLIKELY(!result)) {
    return ExecutionStatus::EXCEPTION;
  }
  return HermesValue::encodeUndefinedValue();
}