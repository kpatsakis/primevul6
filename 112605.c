Variant HHVM_FUNCTION(mb_encoding_aliases, const String& name) {
  const mbfl_encoding *encoding;
  int i = 0;
  encoding = mbfl_name2encoding(name.data());
  if (!encoding) {
    raise_warning("mb_encoding_aliases(): Unknown encoding \"%s\"",
                  name.data());
    return false;
  }

  Array ret = Array::Create();
  if (encoding->aliases != nullptr) {
    while ((*encoding->aliases)[i] != nullptr) {
      ret.append((*encoding->aliases)[i]);
      i++;
    }
  }
  return ret;
}