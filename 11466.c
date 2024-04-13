ciInstance* ciEnv::ArrayIndexOutOfBoundsException_instance() {
  if (_ArrayIndexOutOfBoundsException_instance == NULL) {
    _ArrayIndexOutOfBoundsException_instance
          = get_or_create_exception(_ArrayIndexOutOfBoundsException_handle,
          vmSymbols::java_lang_ArrayIndexOutOfBoundsException());
  }
  return _ArrayIndexOutOfBoundsException_instance;
}