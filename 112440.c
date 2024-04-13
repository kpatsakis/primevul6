static Variant iter_op_impl(VRefParam refParam, OpPtr op, const String& objOp,
                            NonArrayRet nonArray,
                            bool(ArrayData::*pred)() const =
                              &ArrayData::isInvalid) {
  auto& cell = *refParam.wrapped().asCell();
  if (!isArrayType(cell.m_type)) {
    if (cell.m_type == KindOfObject) {
      auto obj = refParam.wrapped().toObject();
      if (obj->instanceof(SystemLib::s_ArrayObjectClass)) {
        return obj->o_invoke_few_args(objOp, 0);
      }
    }
    throw_bad_type_exception("expecting an array");
    return Variant(nonArray);
  }

  auto ad = cell.m_data.parr;
  auto constexpr doCow = !std::is_same<DoCow, NoCow>::value;
  if (doCow && ad->cowCheck() && !(ad->*pred)() &&
      !ad->noCopyOnWrite()) {
    ad = ad->copy();
    if (LIKELY(refParam.isRefData()))
      cellMove(make_tv<KindOfArray>(ad), *refParam.getRefData()->tv());
    else {
      req::ptr<ArrayData> tmp(ad, req::ptr<ArrayData>::NoIncRef{});
      return (ad->*op)();
    }
  }
  return (ad->*op)();
}