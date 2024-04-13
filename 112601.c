Variant HHVM_FUNCTION(mb_preferred_mime_name,
                      const String& encoding) {
  mbfl_no_encoding no_encoding = mbfl_name2no_encoding(encoding.data());
  if (no_encoding == mbfl_no_encoding_invalid) {
    raise_warning("Unknown encoding \"%s\"", encoding.data());
    return false;
  }

  const char *preferred_name = mbfl_no2preferred_mime_name(no_encoding);
  if (preferred_name == nullptr || *preferred_name == '\0') {
    raise_warning("No MIME preferred name corresponding to \"%s\"",
                    encoding.data());
    return false;
  }

  return String(preferred_name, CopyString);
}