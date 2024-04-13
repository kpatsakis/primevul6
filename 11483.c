void ciEnv::dump_replay_data(outputStream* out) {
  GUARDED_VM_ENTRY(
    MutexLocker ml(Compile_lock);
    dump_replay_data_unsafe(out);
  )
}