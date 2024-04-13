inline typename MakeUnsigned<Int>::Type to_unsigned(Int value) {
  FMT_ASSERT(value >= 0, "negative value");
  return static_cast<typename MakeUnsigned<Int>::Type>(value);
}