Variant HHVM_FUNCTION(mb_get_info,
                      const Variant& opt_type) {
  const String type = convertArg(opt_type);

  const mbfl_language *lang = mbfl_no2language(MBSTRG(current_language));
  mbfl_encoding **entry;
  int n;

  char *name;
  if (type.empty() || strcasecmp(type.data(), "all") == 0) {
    Array ret;
    if (MBSTRG(current_internal_encoding) != nullptr &&
        (name = (char *) MBSTRG(current_internal_encoding)->name) != nullptr) {
      ret.set(s_internal_encoding, String(name, CopyString));
    }
    if (MBSTRG(http_input_identify) != nullptr &&
        (name = (char *)MBSTRG(http_input_identify)->name) != nullptr) {
      ret.set(s_http_input, String(name, CopyString));
    }
    if (MBSTRG(current_http_output_encoding) != nullptr &&
        (name = (char *)MBSTRG(current_http_output_encoding)->name) != nullptr) {
      ret.set(s_http_output, String(name, CopyString));
    }
    if (lang != nullptr) {
      if ((name = (char *)mbfl_no_encoding2name
           (lang->mail_charset)) != nullptr) {
        ret.set(s_mail_charset, String(name, CopyString));
      }
      if ((name = (char *)mbfl_no_encoding2name
           (lang->mail_header_encoding)) != nullptr) {
        ret.set(s_mail_header_encoding, String(name, CopyString));
      }
      if ((name = (char *)mbfl_no_encoding2name
           (lang->mail_body_encoding)) != nullptr) {
        ret.set(s_mail_body_encoding, String(name, CopyString));
      }
    }
    ret.set(s_illegal_chars, MBSTRG(illegalchars));
    ret.set(s_encoding_translation,
            MBSTRG(encoding_translation) ? s_On : s_Off);
    if ((name = (char *)mbfl_no_language2name
         (MBSTRG(current_language))) != nullptr) {
      ret.set(s_language, String(name, CopyString));
    }
    n = MBSTRG(current_detect_order_list_size);
    entry = MBSTRG(current_detect_order_list);
    if (n > 0) {
      Array row;
      while (n > 0) {
        if ((name = (char *)(*entry)->name) != nullptr) {
          row.append(String(name, CopyString));
        }
        entry++;
        n--;
      }
      ret.set(s_detect_order, row);
    }
    switch (MBSTRG(current_filter_illegal_mode)) {
    case MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE:
      ret.set(s_substitute_character, s_none);
      break;
    case MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG:
      ret.set(s_substitute_character, s_long);
      break;
    case MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY:
      ret.set(s_substitute_character, s_entity);
      break;
    default:
      ret.set(s_substitute_character,
              MBSTRG(current_filter_illegal_substchar));
    }
    ret.set(s_strict_detection, MBSTRG(strict_detection) ? s_On : s_Off);
    return ret;
  } else if (strcasecmp(type.data(), "internal_encoding") == 0) {
    if (MBSTRG(current_internal_encoding) != nullptr &&
        (name = (char *)MBSTRG(current_internal_encoding)->name) != nullptr) {
      return String(name, CopyString);
    }
  } else if (strcasecmp(type.data(), "http_input") == 0) {
    if (MBSTRG(http_input_identify) != nullptr &&
        (name = (char *)MBSTRG(http_input_identify)->name) != nullptr) {
      return String(name, CopyString);
    }
  } else if (strcasecmp(type.data(), "http_output") == 0) {
    if (MBSTRG(current_http_output_encoding) != nullptr &&
        (name = (char *)MBSTRG(current_http_output_encoding)->name) != nullptr) {
      return String(name, CopyString);
    }
  } else if (strcasecmp(type.data(), "mail_charset") == 0) {
    if (lang != nullptr &&
        (name = (char *)mbfl_no_encoding2name
         (lang->mail_charset)) != nullptr) {
      return String(name, CopyString);
    }
  } else if (strcasecmp(type.data(), "mail_header_encoding") == 0) {
    if (lang != nullptr &&
        (name = (char *)mbfl_no_encoding2name
         (lang->mail_header_encoding)) != nullptr) {
      return String(name, CopyString);
    }
  } else if (strcasecmp(type.data(), "mail_body_encoding") == 0) {
    if (lang != nullptr &&
        (name = (char *)mbfl_no_encoding2name
         (lang->mail_body_encoding)) != nullptr) {
      return String(name, CopyString);
    }
  } else if (strcasecmp(type.data(), "illegal_chars") == 0) {
    return MBSTRG(illegalchars);
  } else if (strcasecmp(type.data(), "encoding_translation") == 0) {
    return MBSTRG(encoding_translation) ? "On" : "Off";
  } else if (strcasecmp(type.data(), "language") == 0) {
    if ((name = (char *)mbfl_no_language2name
         (MBSTRG(current_language))) != nullptr) {
      return String(name, CopyString);
    }
  } else if (strcasecmp(type.data(), "detect_order") == 0) {
    n = MBSTRG(current_detect_order_list_size);
    entry = MBSTRG(current_detect_order_list);
    if (n > 0) {
      Array ret;
      while (n > 0) {
        name = (char *)(*entry)->name;
        if (name) {
          ret.append(String(name, CopyString));
        }
        entry++;
        n--;
      }
    }
  } else if (strcasecmp(type.data(), "substitute_character") == 0) {
    if (MBSTRG(current_filter_illegal_mode) ==
        MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE) {
      return s_none;
    } else if (MBSTRG(current_filter_illegal_mode) ==
               MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG) {
      return s_long;
    } else if (MBSTRG(current_filter_illegal_mode) ==
               MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY) {
      return s_entity;
    } else {
      return MBSTRG(current_filter_illegal_substchar);
    }
  } else if (strcasecmp(type.data(), "strict_detection") == 0) {
    return MBSTRG(strict_detection) ? s_On : s_Off;
  }
  return false;
}