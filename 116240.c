const Char *BasicFormatter<Char, ArgFormatter>::format(
    const Char *&format_str, const internal::Arg &arg) {
  using internal::Arg;
  const Char *s = format_str;
  typename ArgFormatter::SpecType spec;
  if (*s == ':') {
    if (arg.type == Arg::CUSTOM) {
      arg.custom.format(this, arg.custom.value, &s);
      return s;
    }
    ++s;
    // Parse fill and alignment.
    if (Char c = *s) {
      const Char *p = s + 1;
      spec.align_ = ALIGN_DEFAULT;
      do {
        switch (*p) {
          case '<':
            spec.align_ = ALIGN_LEFT;
            break;
          case '>':
            spec.align_ = ALIGN_RIGHT;
            break;
          case '=':
            spec.align_ = ALIGN_NUMERIC;
            break;
          case '^':
            spec.align_ = ALIGN_CENTER;
            break;
        }
        if (spec.align_ != ALIGN_DEFAULT) {
          if (p != s) {
            if (c == '}') break;
            if (c == '{')
              FMT_THROW(FormatError("invalid fill character '{'"));
            s += 2;
            spec.fill_ = c;
          } else ++s;
          if (spec.align_ == ALIGN_NUMERIC)
            require_numeric_argument(arg, '=');
          break;
        }
      } while (--p >= s);
    }

    // Parse sign.
    switch (*s) {
      case '+':
        check_sign(s, arg);
        spec.flags_ |= SIGN_FLAG | PLUS_FLAG;
        break;
      case '-':
        check_sign(s, arg);
        spec.flags_ |= MINUS_FLAG;
        break;
      case ' ':
        check_sign(s, arg);
        spec.flags_ |= SIGN_FLAG;
        break;
    }

    if (*s == '#') {
      require_numeric_argument(arg, '#');
      spec.flags_ |= HASH_FLAG;
      ++s;
    }

    // Parse zero flag.
    if (*s == '0') {
      require_numeric_argument(arg, '0');
      spec.align_ = ALIGN_NUMERIC;
      spec.fill_ = '0';
      ++s;
    }

    // Parse width.
    if ('0' <= *s && *s <= '9') {
      spec.width_ = internal::parse_nonnegative_int(s);
    } else if (*s == '{') {
      ++s;
      Arg width_arg = internal::is_name_start(*s) ?
            parse_arg_name(s) : parse_arg_index(s);
      if (*s++ != '}')
        FMT_THROW(FormatError("invalid format string"));
      ULongLong value = 0;
      switch (width_arg.type) {
      case Arg::INT:
        if (width_arg.int_value < 0)
          FMT_THROW(FormatError("negative width"));
        value = width_arg.int_value;
        break;
      case Arg::UINT:
        value = width_arg.uint_value;
        break;
      case Arg::LONG_LONG:
        if (width_arg.long_long_value < 0)
          FMT_THROW(FormatError("negative width"));
        value = width_arg.long_long_value;
        break;
      case Arg::ULONG_LONG:
        value = width_arg.ulong_long_value;
        break;
      default:
        FMT_THROW(FormatError("width is not integer"));
      }
      unsigned max_int = (std::numeric_limits<int>::max)();
      if (value > max_int)
        FMT_THROW(FormatError("number is too big"));
      spec.width_ = static_cast<int>(value);
    }

    // Parse precision.
    if (*s == '.') {
      ++s;
      spec.precision_ = 0;
      if ('0' <= *s && *s <= '9') {
        spec.precision_ = internal::parse_nonnegative_int(s);
      } else if (*s == '{') {
        ++s;
        Arg precision_arg = internal::is_name_start(*s) ?
              parse_arg_name(s) : parse_arg_index(s);
        if (*s++ != '}')
          FMT_THROW(FormatError("invalid format string"));
        ULongLong value = 0;
        switch (precision_arg.type) {
          case Arg::INT:
            if (precision_arg.int_value < 0)
              FMT_THROW(FormatError("negative precision"));
            value = precision_arg.int_value;
            break;
          case Arg::UINT:
            value = precision_arg.uint_value;
            break;
          case Arg::LONG_LONG:
            if (precision_arg.long_long_value < 0)
              FMT_THROW(FormatError("negative precision"));
            value = precision_arg.long_long_value;
            break;
          case Arg::ULONG_LONG:
            value = precision_arg.ulong_long_value;
            break;
          default:
            FMT_THROW(FormatError("precision is not integer"));
        }
        unsigned max_int = (std::numeric_limits<int>::max)();
        if (value > max_int)
          FMT_THROW(FormatError("number is too big"));
        spec.precision_ = static_cast<int>(value);
      } else {
        FMT_THROW(FormatError("missing precision specifier"));
      }
      if (arg.type <= Arg::LAST_INTEGER_TYPE || arg.type == Arg::POINTER) {
        FMT_THROW(FormatError(
            fmt::format("precision not allowed in {} format specifier",
            arg.type == Arg::POINTER ? "pointer" : "integer")));
      }
    }

    // Parse type.
    if (*s != '}' && *s)
      spec.type_ = static_cast<char>(*s++);
  }

  if (*s++ != '}')
    FMT_THROW(FormatError("missing '}' in format string"));

  // Format argument.
  ArgFormatter(*this, spec, s - 1).visit(arg);
  return s;
}