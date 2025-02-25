void RaiseFallbackException(const char* message) {
  if (fallback_exception_class != nullptr) {
    PyErr_SetString(fallback_exception_class, message);
    return;
  }

  PyErr_SetString(
      PyExc_RuntimeError,
      tensorflow::strings::StrCat(
          "Fallback exception type not set, attempting to fallback due to ",
          message)
          .data());
}