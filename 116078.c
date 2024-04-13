  static void format_custom_arg(
      void *formatter, const void *arg, void *format_str_ptr) {
    format_arg(*static_cast<Formatter*>(formatter),
               *static_cast<const Char**>(format_str_ptr),
               *static_cast<const T*>(arg));
  }