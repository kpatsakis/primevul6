static bool php_mbfl_encoding_detect(const Variant& var,
                                     mbfl_encoding_detector *identd,
                                     mbfl_string *string) {
  if (var.isArray() || var.is(KindOfObject)) {
    Array items = var.toArray();
    for (ArrayIter iter(items); iter; ++iter) {
      if (php_mbfl_encoding_detect(iter.second(), identd, string)) {
        return true;
      }
    }
  } else if (var.isString()) {
    String svar = var.toString();
    string->val = (unsigned char *)svar.data();
    string->len = svar.size();
    if (mbfl_encoding_detector_feed(identd, string)) {
      return true;
    }
  }
  return false;
}