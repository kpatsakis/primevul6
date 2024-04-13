hermesBuiltinArraySpread(void *, Runtime *runtime, NativeArgs args) {
  Handle<JSArray> target = args.dyncastArg<JSArray>(0);
  // To be safe, check for non-arrays.
  if (!target) {
    return runtime->raiseTypeError(
        "HermesBuiltin.arraySpread requires an array target");
  }

  MutableHandle<> nextValue{runtime};

  Handle<JSArray> arr = args.dyncastArg<JSArray>(1);
  if (arr) {
    // Copying from an array, first check and make sure that
    // `arr[Symbol.iterator]` hasn't been changed by the user.
    NamedPropertyDescriptor desc;
    PseudoHandle<JSObject> propObj =
        createPseudoHandle(JSObject::getNamedDescriptor(
            arr,
            runtime,
            Predefined::getSymbolID(Predefined::SymbolIterator),
            desc));
    if (LLVM_LIKELY(propObj) && LLVM_LIKELY(!desc.flags.proxyObject)) {
      PseudoHandle<> slotValue = createPseudoHandle(
          JSObject::getNamedSlotValue(propObj.get(), runtime, desc));
      propObj.invalidate();
      if (LLVM_LIKELY(
              slotValue->getRaw() == runtime->arrayPrototypeValues.getRaw())) {
        slotValue.invalidate();
        auto nextIndex = args.getArg(2).getNumberAs<JSArray::size_type>();
        MutableHandle<> idxHandle{runtime};
        for (JSArray::size_type i = 0; i < JSArray::getLength(*arr); ++i) {
          // Fast path: look up the property in indexed storage.
          nextValue = arr->at(runtime, i);
          if (LLVM_UNLIKELY(nextValue->isEmpty())) {
            // Slow path, just run the full getComputed_RJS path.
            // Runs when there is a hole, accessor, non-regular property, etc.
            GCScopeMarkerRAII marker{runtime};
            idxHandle = HermesValue::encodeNumberValue(i);
            CallResult<HermesValue> valueRes =
                JSObject::getComputed_RJS(arr, runtime, idxHandle);
            if (LLVM_UNLIKELY(valueRes == ExecutionStatus::EXCEPTION)) {
              return ExecutionStatus::EXCEPTION;
            }
            nextValue = *valueRes;
          }
          // It is valid to use setElementAt here because we know that
          // `target` was created immediately prior to running the spread
          // and no non-standard properties were added to it,
          // because the only actions that can be performed between array
          // creation and running this spread are DefineOwnProperty calls with
          // standard flags (as well as other spread operations, which do the
          // same thing).
          JSArray::setElementAt(target, runtime, nextIndex, nextValue);
          ++nextIndex;
        }

        if (LLVM_UNLIKELY(
                JSArray::setLengthProperty(target, runtime, nextIndex) ==
                ExecutionStatus::EXCEPTION)) {
          return ExecutionStatus::EXCEPTION;
        }

        return HermesValue::encodeNumberValue(nextIndex);
      }
    }
  }

  // 3. Let iteratorRecord be ? GetIterator(spreadObj).
  auto iteratorRecordRes = getIterator(runtime, args.getArgHandle(1));
  if (LLVM_UNLIKELY(iteratorRecordRes == ExecutionStatus::EXCEPTION)) {
    return ExecutionStatus::EXCEPTION;
  }
  IteratorRecord iteratorRecord = *iteratorRecordRes;

  MutableHandle<> nextIndex{runtime, args.getArg(2)};

  // 4. Repeat,
  for (GCScopeMarkerRAII marker{runtime}; /* nothing */; marker.flush()) {
    // a. Let next be ? IteratorStep(iteratorRecord).
    auto nextRes = iteratorStep(runtime, iteratorRecord);
    if (LLVM_UNLIKELY(nextRes == ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }
    Handle<JSObject> next = *nextRes;

    // b. If next is false, return nextIndex.
    if (!next) {
      return nextIndex.getHermesValue();
    }
    // c. Let nextValue be ? IteratorValue(next).
    auto nextItemRes = JSObject::getNamed_RJS(
        next, runtime, Predefined::getSymbolID(Predefined::value));
    if (LLVM_UNLIKELY(nextItemRes == ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }
    nextValue = *nextItemRes;

    // d. Let status be CreateDataProperty(array,
    //    ToString(ToUint32(nextIndex)), nextValue).
    // e. Assert: status is true.
    if (LLVM_UNLIKELY(
            JSArray::putComputed_RJS(target, runtime, nextIndex, nextValue) ==
            ExecutionStatus::EXCEPTION)) {
      return ExecutionStatus::EXCEPTION;
    }

    // f. Let nextIndex be nextIndex + 1.
    nextIndex = HermesValue::encodeNumberValue(nextIndex->getNumber() + 1);
  }

  return nextIndex.getHermesValue();
}