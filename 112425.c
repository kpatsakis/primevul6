static void php_array_replace_recursive(PointerSet &seen, bool check,
                                        Array &arr1, const Array& arr2) {
  if (arr1.get() == arr2.get()) {
    // This is an optimization, but it also avoids an assert in
    // setWithRef (Variant::setWithRef asserts that its source
    // and destination are not the same).
    // If the arrays are self recursive, this does change the behavior
    // slightly - it skips the "recursion detected" warning.
    return;
  }

  if (check && !seen.insert((void*)arr1.get()).second) {
    raise_warning("array_replace_recursive(): recursion detected");
    return;
  }

  for (ArrayIter iter(arr2); iter; ++iter) {
    Variant key = iter.first();
    const Variant& value = iter.secondRef();
    if (arr1.exists(key, true) && value.isArray()) {
      Variant &v = arr1.lvalAt(key, AccessFlags::Key);
      if (v.isArray()) {
        Array subarr1 = v.toArray();
        const ArrNR& arr_value = value.toArrNR();
        php_array_replace_recursive(seen, couldRecur(v, subarr1.get()),
                                    subarr1, arr_value);
        v = subarr1;
      } else {
        arr1.set(key, value, true);
      }
    } else {
      arr1.setWithRef(key, value, true);
    }
  }

  if (check) {
    seen.erase((void*)arr1.get());
  }
}