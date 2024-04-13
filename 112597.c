static Variant php_mbfl_convert(const Variant& var,
                                mbfl_buffer_converter *convd,
                                mbfl_string *string,
                                mbfl_string *result) {
  if (var.isArray()) {
    Array ret = empty_array();
    Array items = var.toArray();
    for (ArrayIter iter(items); iter; ++iter) {
      ret.set(iter.first(),
              php_mbfl_convert(iter.second(), convd, string, result));
    }
    return ret;
  }

  if (var.is(KindOfObject)) {
    Object obj = var.toObject();
    Array items = var.toArray();
    for (ArrayIter iter(items); iter; ++iter) {
      obj->o_set(iter.first().toString(),
                 php_mbfl_convert(iter.second(), convd, string, result));
    }
    return var; // which still has obj
  }

  if (var.isString()) {
    String svar = var.toString();
    string->val = (unsigned char *)svar.data();
    string->len = svar.size();
    mbfl_string *ret =
      mbfl_buffer_converter_feed_result(convd, string, result);
    return String(reinterpret_cast<char*>(ret->val), ret->len, AttachString);
  }

  return var;
}