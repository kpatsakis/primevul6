static char *php_mb_convert_encoding(const char *input, size_t length,
                                     const char *_to_encoding,
                                     const char *_from_encodings,
                                     unsigned int *output_len) {
  mbfl_string string, result, *ret;
  mbfl_encoding *from_encoding, *to_encoding;
  mbfl_buffer_converter *convd;
  int size;
  mbfl_encoding **list;
  char *output = nullptr;

  if (output_len) {
    *output_len = 0;
  }
  if (!input) {
    return nullptr;
  }

  /* new encoding */
  if (_to_encoding && strlen(_to_encoding)) {
    to_encoding = (mbfl_encoding*) mbfl_name2encoding(_to_encoding);
    if (to_encoding == nullptr) {
      raise_warning("Unknown encoding \"%s\"", _to_encoding);
      return nullptr;
    }
  } else {
    to_encoding = MBSTRG(current_internal_encoding);
  }

  /* initialize string */
  mbfl_string_init(&string);
  mbfl_string_init(&result);
  from_encoding = MBSTRG(current_internal_encoding);
  string.no_encoding = from_encoding->no_encoding;
  string.no_language = MBSTRG(current_language);
  string.val = (unsigned char *)input;
  string.len = length;

  /* pre-conversion encoding */
  if (_from_encodings) {
    list = nullptr;
    size = 0;
    php_mb_parse_encoding_list(_from_encodings, strlen(_from_encodings),
                               &list, &size, 0);
    if (size == 1) {
      from_encoding = *list;
      string.no_encoding = from_encoding->no_encoding;
    } else if (size > 1) {
      /* auto detect */
      from_encoding = (mbfl_encoding*) mbfl_identify_encoding2(&string,
                                              (const mbfl_encoding**) list,
                                              size, MBSTRG(strict_detection));
      if (from_encoding != nullptr) {
        string.no_encoding = from_encoding->no_encoding;
      } else {
        raise_warning("Unable to detect character encoding");
        from_encoding = (mbfl_encoding*) &mbfl_encoding_pass;
        to_encoding = from_encoding;
        string.no_encoding = from_encoding->no_encoding;
      }
    } else {
      raise_warning("Illegal character encoding specified");
    }
    if (list != nullptr) {
      req::free(list);
    }
  }

  /* initialize converter */
  convd = mbfl_buffer_converter_new2(from_encoding, to_encoding, string.len);
  if (convd == nullptr) {
    raise_warning("Unable to create character encoding converter");
    return nullptr;
  }
  mbfl_buffer_converter_illegal_mode
    (convd, MBSTRG(current_filter_illegal_mode));
  mbfl_buffer_converter_illegal_substchar
    (convd, MBSTRG(current_filter_illegal_substchar));

  /* do it */
  ret = mbfl_buffer_converter_feed_result(convd, &string, &result);
  if (ret) {
    if (output_len) {
      *output_len = ret->len;
    }
    output = (char *)ret->val;
  }

  MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
  mbfl_buffer_converter_delete(convd);
  return output;
}