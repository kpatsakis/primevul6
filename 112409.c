TypedValue HHVM_FUNCTION(array_slice,
                         TypedValue cell_input,
                         int64_t offset,
                         const Variant& length /* = null_variant */,
                         bool preserve_keys /* = false */) {
  if (UNLIKELY(!isContainer(cell_input))) {
    raise_warning("Invalid operand type was used: %s expects "
                  "an array or collection as argument 1",
                  __FUNCTION__+2);
    return make_tv<KindOfNull>();
  }
  int64_t len = length.isNull() ? 0x7FFFFFFF : length.toInt64();

  const int64_t num_in = getContainerSize(cell_input);
  if (offset > num_in) {
    offset = num_in;
  } else if (offset < 0 && (offset = (num_in + offset)) < 0) {
    offset = 0;
  }

  auto const maxLen = num_in - offset;
  if (len < 0) {
    len = maxLen + len;
  } else if (len > maxLen) {
    len = maxLen;
  }

  if (len <= 0) {
    return make_tv<KindOfPersistentArray>(staticEmptyArray());
  }

  bool input_is_packed = isPackedContainer(cell_input);

  // If the slice covers the entire input container, we can just nop when
  // preserve_keys is true, or when preserve_keys is false but the container
  // is packed so we know the keys already map to [0,N].
  if (offset == 0 && len == num_in && (preserve_keys || input_is_packed)) {
    if (isArrayType(cell_input.m_type)) {
      return tvReturn(Variant(cell_input.m_data.parr));
    }
    return tvReturn(cell_input.m_data.pobj->toArray());
  }

  int pos = 0;
  ArrayIter iter(cell_input);
  for (; pos < offset && iter; ++pos, ++iter) {}

  if (input_is_packed && (offset == 0 || !preserve_keys)) {
    PackedArrayInit ret(len);
    for (; pos < (offset + len) && iter; ++pos, ++iter) {
      ret.appendWithRef(iter.secondRefPlus());
    }
    return tvReturn(ret.toVariant());
  }

  // Otherwise PackedArrayInit can't be used because non-numeric keys are
  // preserved even when preserve_keys is false
  Array ret = Array::attach(PackedArray::MakeReserve(len));
  for (; pos < (offset + len) && iter; ++pos, ++iter) {
    Variant key(iter.first());
    bool doAppend = !preserve_keys && key.isNumeric();
    const Variant& v = iter.secondRefPlus();
    if (doAppend) {
      ret.appendWithRef(v);
    } else {
      ret.setWithRef(key, v, true);
    }
  }
  return tvReturn(std::move(ret));
}