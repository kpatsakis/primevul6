void ciEnv::notice_inlined_method(ciMethod* method) {
  _num_inlined_bytecodes += method->code_size_for_inlining();
}