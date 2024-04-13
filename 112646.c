Array HHVM_FUNCTION(mb_list_encodings) {
  Array ret;
  int i = 0;
  const mbfl_encoding **encodings = mbfl_get_supported_encodings();
  const mbfl_encoding *encoding;
  while ((encoding = encodings[i++]) != nullptr) {
    ret.append(String(encoding->name, CopyString));
  }
  return ret;
}