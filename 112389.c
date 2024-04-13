TypedValue HHVM_FUNCTION(array_fill,
                         int start_index,
                         int num,
                         const Variant& value) {
  if (num < 0) {
    throw_invalid_argument("Number of elements can't be negative");
    return make_tv<KindOfBoolean>(false);
  }

  if (start_index == 0) {
    PackedArrayInit pai(num, CheckAllocation{});
    for (size_t k = 0; k < num; k++) {
      pai.append(value);
    }
    return tvReturn(pai.toVariant());
  } else {
    ArrayInit ret(num, ArrayInit::Mixed{}, CheckAllocation{});
    ret.set(start_index, value);
    for (int i = num - 1; i > 0; i--) {
      ret.append(value);
    }
    return tvReturn(ret.toVariant());
  }
}