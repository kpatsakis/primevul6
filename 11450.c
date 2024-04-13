ciMethod* ciEnv::get_method_by_index(constantPoolHandle cpool,
                                     int index, Bytecodes::Code bc,
                                     ciInstanceKlass* accessor) {
  GUARDED_VM_ENTRY(return get_method_by_index_impl(cpool, index, bc, accessor);)
}