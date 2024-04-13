static void php_array_merge_recursive(PointerSet &seen, bool check,
                                      Array &arr1, const Array& arr2) {
  if (check && !seen.insert((void*)arr1.get()).second) {
    raise_warning("array_merge_recursive(): recursion detected");
    return;
  }

  for (ArrayIter iter(arr2); iter; ++iter) {
    Variant key(iter.first());
    const Variant& value(iter.secondRef());
    if (key.isNumeric()) {
      arr1.appendWithRef(value);
    } else if (arr1.exists(key, true)) {
      // There is no need to do toKey() conversion, for a key that is already
      // in the array.
      Variant &v = arr1.lvalAt(key, AccessFlags::Key);
      auto subarr1 = v.toArray().copy();
      php_array_merge_recursive(seen,
                                couldRecur(v, subarr1.get()),
                                subarr1,
                                value.toArray());
      v.unset(); // avoid contamination of the value that was strongly bound
      v = subarr1;
    } else {
      arr1.setWithRef(key, value, true);
    }
  }

  if (check) {
    seen.erase((void*)arr1.get());
  }
}