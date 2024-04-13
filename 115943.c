void BasicWriter<Char>::write_str(
    const internal::Arg::StringValue<StrChar> &s, const Spec &spec) {
  // Check if StrChar is convertible to Char.
  internal::CharTraits<Char>::convert(StrChar());
  if (spec.type_ && spec.type_ != 's')
    internal::report_unknown_type(spec.type_, "string");
  const StrChar *str_value = s.value;
  std::size_t str_size = s.size;
  if (str_size == 0) {
    if (!str_value) {
      FMT_THROW(FormatError("string pointer is null"));
    }
  }
  std::size_t precision = static_cast<std::size_t>(spec.precision_);
  if (spec.precision_ >= 0 && precision < str_size)
    str_size = precision;
  write_str(str_value, str_size, spec);
}