ciInstance* ciEnv::unloaded_ciinstance() {
  GUARDED_VM_ENTRY(return _factory->get_unloaded_object_constant();)
}