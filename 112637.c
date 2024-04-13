Variant HHVM_FUNCTION(mb_http_output,
                      const Variant& opt_encoding) {
  const String encoding_name = convertArg(opt_encoding);

  if (encoding_name.empty()) {
    char *name = (char *)(MBSTRG(current_http_output_encoding)->name);
    if (name != nullptr) {
      return String(name, CopyString);
    }
    return false;
  }

  mbfl_encoding *encoding =
    (mbfl_encoding*) mbfl_name2encoding(encoding_name.data());
  if (encoding == nullptr) {
    raise_warning("Unknown encoding \"%s\"", encoding_name.data());
    return false;
  }
  MBSTRG(current_http_output_encoding) = encoding;
  return true;
}