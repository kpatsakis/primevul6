ciField* ciEnv::get_field_by_index(ciInstanceKlass* accessor,
                                   int index) {
  GUARDED_VM_ENTRY(return get_field_by_index_impl(accessor, index);)
}