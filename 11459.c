bool ciEnv::jvmti_state_changed() const {
  if (!_jvmti_can_access_local_variables &&
      JvmtiExport::can_access_local_variables()) {
    return true;
  }
  if (!_jvmti_can_hotswap_or_post_breakpoint &&
      JvmtiExport::can_hotswap_or_post_breakpoint()) {
    return true;
  }
  if (!_jvmti_can_post_on_exceptions &&
      JvmtiExport::can_post_on_exceptions()) {
    return true;
  }
  if (!_jvmti_can_pop_frame &&
      JvmtiExport::can_pop_frame()) {
    return true;
  }
  return false;
}