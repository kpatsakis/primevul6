ciInstance* ciEnv::the_null_string() {
  if (_the_null_string == NULL) {
    VM_ENTRY_MARK;
    _the_null_string = get_object(Universe::the_null_string())->as_instance();
  }
  return _the_null_string;
}