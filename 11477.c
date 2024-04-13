ciInstance* ciEnv::ClassCastException_instance() {
  if (_ClassCastException_instance == NULL) {
    _ClassCastException_instance
          = get_or_create_exception(_ClassCastException_handle,
          vmSymbols::java_lang_ClassCastException());
  }
  return _ClassCastException_instance;
}