bool ciEnv::is_in_vm() {
  return JavaThread::current()->thread_state() == _thread_in_vm;
}