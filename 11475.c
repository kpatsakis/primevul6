void ciEnv::record_out_of_memory_failure() {
  // If memory is low, we stop compiling methods.
  record_method_not_compilable("out of memory");
}