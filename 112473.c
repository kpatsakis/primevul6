TypedValue HHVM_FUNCTION(array_chunk,
                         const Variant& input,
                         int chunkSize,
                         bool preserve_keys /* = false */) {
  const auto& cellInput = *input.asCell();
  if (UNLIKELY(!isContainer(cellInput))) {
    raise_warning("Invalid operand type was used: %s expects "
                  "an array or collection as argument 1", __FUNCTION__+2);
    return make_tv<KindOfNull>();
  }

  if (chunkSize < 1) {
    throw_invalid_argument("size: %d", chunkSize);
    return make_tv<KindOfNull>();
  }

  auto const retSize = (getContainerSize(cellInput) / chunkSize) + 1;
  PackedArrayInit ret(retSize);
  Array chunk;
  int current = 0;
  for (ArrayIter iter(cellInput); iter; ++iter) {
    if (preserve_keys) {
      chunk.setWithRef(iter.first(), iter.secondRefPlus(), true);
    } else {
      chunk.appendWithRef(iter.secondRefPlus());
    }
    if ((++current % chunkSize) == 0) {
      ret.append(chunk);
      chunk.clear();
    }
  }
  if (!chunk.empty()) {
    ret.append(chunk);
  }

  return tvReturn(ret.toVariant());
}