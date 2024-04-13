ciKlass* ciEnv::get_klass_by_index(constantPoolHandle cpool,
                                   int index,
                                   bool& is_accessible,
                                   ciInstanceKlass* accessor) {
  GUARDED_VM_ENTRY(return get_klass_by_index_impl(cpool, index, is_accessible, accessor);)
}