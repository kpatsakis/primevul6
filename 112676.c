Variant HHVM_FUNCTION(mb_convert_variables,
                      const String& to_encoding,
                      const Variant& from_encoding,
                      Variant& vars,
                      const Array& args /* = null_array */) {
  mbfl_string string, result;
  mbfl_encoding *_from_encoding, *_to_encoding;
  mbfl_encoding_detector *identd;
  mbfl_buffer_converter *convd;
  int elistsz;
  mbfl_encoding **elist;
  char *name;

  /* new encoding */
  _to_encoding = (mbfl_encoding*) mbfl_name2encoding(to_encoding.data());
  if (_to_encoding == nullptr) {
    raise_warning("Unknown encoding \"%s\"", to_encoding.data());
    return false;
  }

  /* initialize string */
  mbfl_string_init(&string);
  mbfl_string_init(&result);
  _from_encoding = MBSTRG(current_internal_encoding);
  string.no_encoding = _from_encoding->no_encoding;
  string.no_language = MBSTRG(current_language);

  /* pre-conversion encoding */
  elist = nullptr;
  elistsz = 0;
  php_mb_parse_encoding(from_encoding, &elist, &elistsz, false);
  if (elistsz <= 0) {
    _from_encoding = (mbfl_encoding*) &mbfl_encoding_pass;
  } else if (elistsz == 1) {
    _from_encoding = *elist;
  } else {
    /* auto detect */
    _from_encoding = nullptr;
    identd = mbfl_encoding_detector_new2((const mbfl_encoding**) elist, elistsz,
                                        MBSTRG(strict_detection));
    if (identd != nullptr) {
      for (int n = -1; n < args.size(); n++) {
        if (php_mbfl_encoding_detect(n < 0 ? vars : args[n],
                                     identd, &string)) {
          break;
        }
      }
      _from_encoding = (mbfl_encoding*) mbfl_encoding_detector_judge2(identd);
      mbfl_encoding_detector_delete(identd);
    }

    if (_from_encoding == nullptr) {
      raise_warning("Unable to detect encoding");
      _from_encoding = (mbfl_encoding*) &mbfl_encoding_pass;
    }
  }
  if (elist != nullptr) {
    req::free(elist);
  }

  /* create converter */
  convd = nullptr;
  if (_from_encoding != &mbfl_encoding_pass) {
    convd = mbfl_buffer_converter_new2(_from_encoding, _to_encoding, 0);
    if (convd == nullptr) {
      raise_warning("Unable to create converter");
      return false;
    }
    mbfl_buffer_converter_illegal_mode
      (convd, MBSTRG(current_filter_illegal_mode));
    mbfl_buffer_converter_illegal_substchar
      (convd, MBSTRG(current_filter_illegal_substchar));
  }

  /* convert */
  if (convd != nullptr) {
    vars = php_mbfl_convert(vars, convd, &string, &result);
    for (int n = 0; n < args.size(); n++) {
      const_cast<Array&>(args).set(n, php_mbfl_convert(args[n], convd,
                                                        &string, &result));
    }
    MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
    mbfl_buffer_converter_delete(convd);
  }


  if (_from_encoding != nullptr) {
    name = (char*) _from_encoding->name;
    if (name != nullptr) {
      return String(name, CopyString);
    }
  }
  return false;
}