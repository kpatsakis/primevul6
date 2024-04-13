static mbfl_encoding* _php_mb_encoding_handler_ex
(const php_mb_encoding_handler_info_t *info, Array& arg, char *res) {
  char *var, *val;
  const char *s1, *s2;
  char *strtok_buf = nullptr, **val_list = nullptr;
  int n, num, *len_list = nullptr;
  unsigned int val_len;
  mbfl_string string, resvar, resval;
  mbfl_encoding *from_encoding = nullptr;
  mbfl_encoding_detector *identd = nullptr;
  mbfl_buffer_converter *convd = nullptr;

  mbfl_string_init_set(&string, info->to_language,
                       info->to_encoding->no_encoding);
  mbfl_string_init_set(&resvar, info->to_language,
                       info->to_encoding->no_encoding);
  mbfl_string_init_set(&resval, info->to_language,
                       info->to_encoding->no_encoding);

  if (!res || *res == '\0') {
    goto out;
  }

  /* count the variables(separators) contained in the "res".
   * separator may contain multiple separator chars.
   */
  num = 1;
  for (s1=res; *s1 != '\0'; s1++) {
    for (s2=info->separator; *s2 != '\0'; s2++) {
      if (*s1 == *s2) {
        num++;
      }
    }
  }
  num *= 2; /* need space for variable name and value */

  val_list = (char **)req::calloc_noptrs(num, sizeof(char *));
  len_list = (int *)req::calloc_noptrs(num, sizeof(int));

  /* split and decode the query */
  n = 0;
  strtok_buf = nullptr;
  var = strtok_r(res, info->separator, &strtok_buf);
  while (var)  {
    val = strchr(var, '=');
    if (val) { /* have a value */
      len_list[n] = url_decode_ex(var, val-var);
      val_list[n] = var;
      n++;

      *val++ = '\0';
      val_list[n] = val;
      len_list[n] = url_decode_ex(val, strlen(val));
    } else {
      len_list[n] = url_decode_ex(var, strlen(var));
      val_list[n] = var;
      n++;

      val_list[n] = const_cast<char*>("");
      len_list[n] = 0;
    }
    n++;
    var = strtok_r(nullptr, info->separator, &strtok_buf);
  }
  num = n; /* make sure to process initilized vars only */

  /* initialize converter */
  if (info->num_from_encodings <= 0) {
    from_encoding = (mbfl_encoding*) &mbfl_encoding_pass;
  } else if (info->num_from_encodings == 1) {
    from_encoding = info->from_encodings[0];
  } else {
    /* auto detect */
    from_encoding = nullptr;
    identd = mbfl_encoding_detector_new
      ((enum mbfl_no_encoding *)info->from_encodings,
       info->num_from_encodings, MBSTRG(strict_detection));
    if (identd) {
      n = 0;
      while (n < num) {
        string.val = (unsigned char *)val_list[n];
        string.len = len_list[n];
        if (mbfl_encoding_detector_feed(identd, &string)) {
          break;
        }
        n++;
      }
      from_encoding = (mbfl_encoding*) mbfl_encoding_detector_judge2(identd);
      mbfl_encoding_detector_delete(identd);
    }
    if (from_encoding == nullptr) {
      if (info->report_errors) {
        raise_warning("Unable to detect encoding");
      }
      from_encoding = (mbfl_encoding*) &mbfl_encoding_pass;
    }
  }

  convd = nullptr;
  if (from_encoding != (mbfl_encoding*) &mbfl_encoding_pass) {
    convd = mbfl_buffer_converter_new2(from_encoding, info->to_encoding, 0);
    if (convd != nullptr) {
      mbfl_buffer_converter_illegal_mode
        (convd, MBSTRG(current_filter_illegal_mode));
      mbfl_buffer_converter_illegal_substchar
        (convd, MBSTRG(current_filter_illegal_substchar));
    } else {
      if (info->report_errors) {
        raise_warning("Unable to create converter");
      }
      goto out;
    }
  }

  /* convert encoding */
  string.no_encoding = from_encoding->no_encoding;

  n = 0;
  while (n < num) {
    string.val = (unsigned char *)val_list[n];
    string.len = len_list[n];
    if (convd != nullptr &&
        mbfl_buffer_converter_feed_result(convd, &string, &resvar) != nullptr) {
      var = (char *)resvar.val;
    } else {
      var = val_list[n];
    }
    n++;
    string.val = (unsigned char *)val_list[n];
    string.len = len_list[n];
    if (convd != nullptr &&
        mbfl_buffer_converter_feed_result(convd, &string, &resval) != nullptr) {
      val = (char *)resval.val;
      val_len = resval.len;
    } else {
      val = val_list[n];
      val_len = len_list[n];
    }
    n++;

    if (val_len > 0) {
      arg.set(String(var, CopyString), String(val, val_len, CopyString));
    }

    if (convd != nullptr) {
      mbfl_string_clear(&resvar);
      mbfl_string_clear(&resval);
    }
  }

out:
  if (convd != nullptr) {
    MBSTRG(illegalchars) += mbfl_buffer_illegalchars(convd);
    mbfl_buffer_converter_delete(convd);
  }
  if (val_list != nullptr) {
    req::free((void *)val_list);
  }
  if (len_list != nullptr) {
    req::free((void *)len_list);
  }

  return from_encoding;
}