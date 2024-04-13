ciInstance* ciEnv::the_min_jint_string() {
  if (_the_min_jint_string == NULL) {
    VM_ENTRY_MARK;
    _the_min_jint_string = get_object(Universe::the_min_jint_string())->as_instance();
  }
  return _the_min_jint_string;
}