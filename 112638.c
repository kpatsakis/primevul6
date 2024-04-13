Variant HHVM_FUNCTION(mb_convert_encoding,
                      const String& str,
                      const String& to_encoding,
                      const Variant& from_encoding /* = uninit_variant */) {
  String encoding = from_encoding.toString();
  if (from_encoding.isArray()) {
    StringBuffer _from_encodings;
    Array encs = from_encoding.toArray();
    for (ArrayIter iter(encs); iter; ++iter) {
      if (!_from_encodings.empty()) {
        _from_encodings.append(",");
      }
      _from_encodings.append(iter.second().toString());
    }
    encoding = _from_encodings.detach();
  }

  unsigned int size;
  char *ret = php_mb_convert_encoding(str.data(), str.size(),
                                      to_encoding.data(),
                                      (!encoding.empty() ?
                                       encoding.data() : nullptr),
                                      &size);
  if (ret != nullptr) {
    return String(ret, size, AttachString);
  }
  return false;
}