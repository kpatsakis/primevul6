ciKlass* ciEnv::find_system_klass(ciSymbol* klass_name) {
  VM_ENTRY_MARK;
  return get_klass_by_name_impl(NULL, constantPoolHandle(), klass_name, false);
}