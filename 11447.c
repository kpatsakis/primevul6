ciInstance* ciEnv::ArrayStoreException_instance() {
  if (_ArrayStoreException_instance == NULL) {
    _ArrayStoreException_instance
          = get_or_create_exception(_ArrayStoreException_handle,
          vmSymbols::java_lang_ArrayStoreException());
  }
  return _ArrayStoreException_instance;
}