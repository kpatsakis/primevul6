void ciEnv::dump_compile_data(outputStream* out) {
  CompileTask* task = this->task();
  Method* method = task->method();
  int entry_bci = task->osr_bci();
  int comp_level = task->comp_level();
  out->print("compile %s %s %s %d %d",
                method->klass_name()->as_quoted_ascii(),
                method->name()->as_quoted_ascii(),
                method->signature()->as_quoted_ascii(),
                entry_bci, comp_level);
  if (compiler_data() != NULL) {
    if (is_c2_compile(comp_level)) { // C2 or Shark
#ifdef COMPILER2
      // Dump C2 inlining data.
      ((Compile*)compiler_data())->dump_inline_data(out);
#endif
    } else if (is_c1_compile(comp_level)) { // C1
#ifdef COMPILER1
      // Dump C1 inlining data.
      ((Compilation*)compiler_data())->dump_inline_data(out);
#endif
    }
  }
  out->cr();
}