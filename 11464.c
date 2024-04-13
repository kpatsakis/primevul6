ciEnv::~ciEnv() {
  CompilerThread* current_thread = CompilerThread::current();
  _factory->remove_symbols();
  // Need safepoint to clear the env on the thread.  RedefineClasses might
  // be reading it.
  GUARDED_VM_ENTRY(current_thread->set_env(NULL);)
}