bool HHVM_FUNCTION(mb_check_encoding,
                   const Variant& opt_var,
                   const Variant& opt_encoding) {
  const String var = convertArg(opt_var);
  const String encoding = convertArg(opt_encoding);

  mbfl_buffer_converter *convd;
  mbfl_no_encoding no_encoding = MBSTRG(current_internal_encoding)->no_encoding;
  mbfl_string string, result, *ret = nullptr;
  long illegalchars = 0;

  if (var.isNull()) {
    return MBSTRG(illegalchars) == 0;
  }

  if (!encoding.isNull()) {
    no_encoding = mbfl_name2no_encoding(encoding.data());
    if (no_encoding == mbfl_no_encoding_invalid ||
        no_encoding == mbfl_no_encoding_pass) {
      raise_warning("Invalid encoding \"%s\"", encoding.data());
      return false;
    }
  }

  convd = mbfl_buffer_converter_new(no_encoding, no_encoding, 0);
  if (convd == nullptr) {
    raise_warning("Unable to create converter");
    return false;
  }
  mbfl_buffer_converter_illegal_mode
    (convd, MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE);
  mbfl_buffer_converter_illegal_substchar
    (convd, 0);

  /* initialize string */
  mbfl_string_init_set(&string, mbfl_no_language_neutral, no_encoding);
  mbfl_string_init(&result);

  string.val = (unsigned char *)var.data();
  string.len = var.size();
  ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
  illegalchars = mbfl_buffer_illegalchars(convd);
  mbfl_buffer_converter_delete(convd);

  if (ret != nullptr) {
    MBSTRG(illegalchars) += illegalchars;
    if (illegalchars == 0 && string.len == ret->len &&
        memcmp((const char *)string.val, (const char *)ret->val,
                string.len) == 0) {
      mbfl_string_clear(&result);
      return true;
    } else {
      mbfl_string_clear(&result);
      return false;
    }
  } else {
    return false;
  }
}