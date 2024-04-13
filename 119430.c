bool GetElementUnexhaustive(const Tensor& t, int i, const std::set<int>& dtypes,
                            complex128* element) {
  if (dtypes.find(t.dtype()) == dtypes.end()) return false;
  switch (t.dtype()) {
    case DT_BFLOAT16:
      *element = complex128(t.flat<bfloat16>()(i));
      return true;
    case DT_HALF:
      *element = complex128(static_cast<double>(t.flat<Eigen::half>()(i)), 0);
      return true;
    case DT_INT32:
      *element = complex128(t.flat<int32>()(i));
      return true;
    case DT_INT64:
      *element = complex128(t.flat<int64>()(i));
      return true;
    case DT_FLOAT:
      *element = complex128(t.flat<float>()(i));
      return true;
    case DT_DOUBLE:
      *element = complex128(t.flat<double>()(i));
      return true;
    case DT_COMPLEX64:
      *element = complex128(t.flat<complex64>()(i));
      return true;
    case DT_COMPLEX128:
      *element = t.flat<complex128>()(i);
      return true;
    default:
      return false;
  }
}