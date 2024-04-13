TypedValue HHVM_FUNCTION(array_map,
                         const Variant& callback,
                         const Variant& arr1,
                         const Array& _argv) {
  VMRegGuard _;
  CallCtx ctx;
  ctx.func = nullptr;
  if (!callback.isNull()) {
    CallerFrame cf;
    vm_decode_function(callback, cf(), false, ctx);
  }
  const auto& cell_arr1 = *arr1.asCell();
  if (UNLIKELY(!isContainer(cell_arr1))) {
    raise_warning("array_map(): Argument #2 should be an array or collection");
    return make_tv<KindOfNull>();
  }
  if (LIKELY(_argv.empty())) {
    // Handle the common case where the caller passed two
    // params (a callback and a container)
    if (!ctx.func) {
      if (isArrayType(cell_arr1.m_type)) {
        return tvReturn(arr1);
      } else {
        return tvReturn(arr1.toArray());
      }
    }
    ArrayInit ret(getContainerSize(cell_arr1), ArrayInit::Map{});
    bool keyConverted = isArrayType(cell_arr1.m_type);
    if (!keyConverted) {
      auto col_type = cell_arr1.m_data.pobj->collectionType();
      keyConverted = !collectionAllowsIntStringKeys(col_type);
    }
    for (ArrayIter iter(arr1); iter; ++iter) {
      Variant result;
      g_context->invokeFuncFew((TypedValue*)&result, ctx, 1,
                               iter.secondRefPlus().asCell());
      // if keyConverted is false, it's possible that ret will have fewer
      // elements than cell_arr1; keys int(1) and string('1') may both be
      // present
      ret.add(iter.first(), result, keyConverted);
    }
    return tvReturn(ret.toVariant());
  }

  // Handle the uncommon case where the caller passed a callback
  // and two or more containers
  req::vector<ArrayIter> iters;
  iters.reserve(_argv.size() + 1);
  size_t maxLen = getContainerSize(cell_arr1);
  iters.emplace_back(cell_arr1);
  for (ArrayIter it(_argv); it; ++it) {
    const auto& c = *it.secondRefPlus().asCell();
    if (UNLIKELY(!isContainer(c))) {
      raise_warning("array_map(): Argument #%d should be an array or "
                    "collection", (int)(iters.size() + 2));
      iters.emplace_back(it.secondRefPlus().toArray());
    } else {
      iters.emplace_back(c);
      size_t len = getContainerSize(c);
      if (len > maxLen) maxLen = len;
    }
  }
  PackedArrayInit ret_ai(maxLen);
  for (size_t k = 0; k < maxLen; k++) {
    PackedArrayInit params_ai(iters.size());
    for (auto& iter : iters) {
      if (iter) {
        params_ai.append(iter.secondRefPlus());
        ++iter;
      } else {
        params_ai.append(init_null_variant);
      }
    }
    Array params = params_ai.toArray();
    if (ctx.func) {
      Variant result;
      g_context->invokeFunc((TypedValue*)&result,
                              ctx.func, params, ctx.this_,
                              ctx.cls, nullptr, ctx.invName);
      ret_ai.append(result);
    } else {
      ret_ai.append(params);
    }
  }
  return tvReturn(ret_ai.toVariant());
}