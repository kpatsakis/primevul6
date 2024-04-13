Variant array_keys_helper(const Variant& input,
                          const Variant& search_value /* = uninit_null */,
                          bool strict /* = false */) {
  const auto& cell_input = *input.asCell();
  if (UNLIKELY(!isContainer(cell_input))) {
    raise_warning("array_keys() expects parameter 1 to be an array "
                  "or collection");
    return init_null();
  }

  if (LIKELY(!search_value.isInitialized())) {
    PackedArrayInit ai(getContainerSize(cell_input));
    for (ArrayIter iter(cell_input); iter; ++iter) {
      ai.append(iter.first());
    }
    return ai.toVariant();
  } else {
    Array ai = Array::attach(PackedArray::MakeReserve(0));
    for (ArrayIter iter(cell_input); iter; ++iter) {
      if ((strict && HPHP::same(iter.secondRefPlus(), search_value)) ||
          (!strict && HPHP::equal(iter.secondRefPlus(), search_value))) {
        ai.append(iter.first());
      }
    }
    return ai;
  }
}