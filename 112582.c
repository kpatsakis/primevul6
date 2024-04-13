Variant HHVM_FUNCTION(mb_substitute_character,
                      const Variant& substrchar /* = uninit_variant */) {
  if (substrchar.isNull()) {
    switch (MBSTRG(current_filter_illegal_mode)) {
    case MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE:
      return "none";
    case MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG:
      return "long";
    case MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY:
      return "entity";
    default:
      return MBSTRG(current_filter_illegal_substchar);
    }
  }

  if (substrchar.isString()) {
    String s = substrchar.toString();
    if (strcasecmp("none", s.data()) == 0) {
      MBSTRG(current_filter_illegal_mode) =
        MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE;
      return true;
    }
    if (strcasecmp("long", s.data()) == 0) {
      MBSTRG(current_filter_illegal_mode) =
        MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG;
      return true;
    }
    if (strcasecmp("entity", s.data()) == 0) {
      MBSTRG(current_filter_illegal_mode) =
        MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY;
      return true;
    }
  }

  int64_t n = substrchar.toInt64();
  if (n < 0xffff && n > 0) {
    MBSTRG(current_filter_illegal_mode) =
      MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR;
    MBSTRG(current_filter_illegal_substchar) = n;
  } else {
    raise_warning("Unknown character.");
    return false;
  }
  return true;
}