  SystemError(int error_code, CStringRef message) {
    init(error_code, message, ArgList());
  }