ciMethod* ciEnv::get_method_from_handle(Method* method) {
  VM_ENTRY_MARK;
  return get_metadata(method)->as_method();
}