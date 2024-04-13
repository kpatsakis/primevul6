void ciEnv::dump_replay_data_unsafe(outputStream* out) {
  ResourceMark rm;
#if INCLUDE_JVMTI
  out->print_cr("JvmtiExport can_access_local_variables %d",     _jvmti_can_access_local_variables);
  out->print_cr("JvmtiExport can_hotswap_or_post_breakpoint %d", _jvmti_can_hotswap_or_post_breakpoint);
  out->print_cr("JvmtiExport can_post_on_exceptions %d",         _jvmti_can_post_on_exceptions);
#endif // INCLUDE_JVMTI

  GrowableArray<ciMetadata*>* objects = _factory->get_ci_metadata();
  out->print_cr("# %d ciObject found", objects->length());
  for (int i = 0; i < objects->length(); i++) {
    objects->at(i)->dump_replay_data(out);
  }
  dump_compile_data(out);
  out->flush();
}