bool* ThreadTapeIsStopped() {
  thread_local bool thread_tape_is_stopped{false};
  return &thread_tape_is_stopped;
}