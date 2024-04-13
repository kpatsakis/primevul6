Variant HHVM_FUNCTION(mb_strpos,
                      const String& haystack,
                      const String& needle,
                      int offset /* = 0 */,
                      const Variant& opt_encoding) {
  const String encoding = convertArg(opt_encoding);

  mbfl_string mbs_haystack;
  mbfl_string_init(&mbs_haystack);
  mbs_haystack.no_language = MBSTRG(current_language);
  mbs_haystack.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  mbs_haystack.val = (unsigned char *)haystack.data();
  mbs_haystack.len = haystack.size();

  mbfl_string mbs_needle;
  mbfl_string_init(&mbs_needle);
  mbs_needle.no_language = MBSTRG(current_language);
  mbs_needle.no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  mbs_needle.val = (unsigned char *)needle.data();
  mbs_needle.len = needle.size();

  if (!encoding.empty()) {
    mbs_haystack.no_encoding = mbs_needle.no_encoding =
      mbfl_name2no_encoding(encoding.data());
    if (mbs_haystack.no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", encoding.data());
      return false;
    }
  }

  if (offset < 0 || offset > mbfl_strlen(&mbs_haystack)) {
    raise_warning("Offset not contained in string.");
    return false;
  }
  if (mbs_needle.len == 0) {
    raise_warning("Empty delimiter.");
    return false;
  }

  int reverse = 0;
  int n = mbfl_strpos(&mbs_haystack, &mbs_needle, offset, reverse);
  if (n >= 0) {
    return n;
  }

  switch (-n) {
  case 1:
    break;
  case 2:
    raise_warning("Needle has not positive length.");
    break;
  case 4:
    raise_warning("Unknown encoding or conversion error.");
    break;
  case 8:
    raise_warning("Argument is empty.");
    break;
  default:
    raise_warning("Unknown error in mb_strpos.");
    break;
  }
  return false;
}