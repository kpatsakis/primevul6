static bool couldRecur(const Variant& v, const ArrayData* arr) {
  return v.isReferenced() ||
    arr->kind() == ArrayData::kGlobalsKind ||
    arr->kind() == ArrayData::kProxyKind;
}