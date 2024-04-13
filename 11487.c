bool ciEnv::should_retain_local_variables() const {
  return _jvmti_can_access_local_variables || _jvmti_can_pop_frame;
}