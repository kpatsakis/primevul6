Variant HHVM_FUNCTION(mb_strrpos,
                      const String& haystack,
                      const String& needle,
                      const Variant& offset /* = 0LL */,
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

  // This hack is so that if the caller puts the encoding in the offset field we
  // attempt to detect it and use that as the encoding.  Ick.
  const char *enc_name = encoding.data();
  long noffset = 0;
  String soffset = offset.toString();
  if (offset.isString()) {
    enc_name = soffset.data();

    int str_flg = 1;
    if (enc_name != nullptr) {
      switch (*enc_name) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case ' ': case '-': case '.':
        break;
      default :
        str_flg = 0;
        break;
      }
    }
    if (str_flg) {
      noffset = offset.toInt32();
      enc_name = encoding.data();
    }
  } else {
    noffset = offset.toInt32();
  }

  if (enc_name != nullptr && *enc_name) {
    mbs_haystack.no_encoding = mbs_needle.no_encoding =
      mbfl_name2no_encoding(enc_name);
    if (mbs_haystack.no_encoding == mbfl_no_encoding_invalid) {
      raise_warning("Unknown encoding \"%s\"", enc_name);
      return false;
    }
  }

  if (mbs_haystack.len <= 0) {
    return false;
  }
  if (mbs_needle.len <= 0) {
    return false;
  }

  if ((noffset > 0 && noffset > mbfl_strlen(&mbs_haystack)) ||
      (noffset < 0 && -noffset > mbfl_strlen(&mbs_haystack))) {
    raise_notice("Offset is greater than the length of haystack string");
    return false;
  }

  int n = mbfl_strpos(&mbs_haystack, &mbs_needle, noffset, 1);
  if (n >= 0) {
    return n;
  }
  return false;
}