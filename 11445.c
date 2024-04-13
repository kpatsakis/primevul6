void ciEnv::cache_jvmti_state() {
  VM_ENTRY_MARK;
  // Get Jvmti capabilities under lock to get consistant values.
  MutexLocker mu(JvmtiThreadState_lock);
  _jvmti_can_hotswap_or_post_breakpoint = JvmtiExport::can_hotswap_or_post_breakpoint();
  _jvmti_can_access_local_variables     = JvmtiExport::can_access_local_variables();
  _jvmti_can_post_on_exceptions         = JvmtiExport::can_post_on_exceptions();
  _jvmti_can_pop_frame                  = JvmtiExport::can_pop_frame();
}