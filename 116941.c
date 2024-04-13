tensorflow::gtl::CompactPointerSet<TFE_Py_Tape*>* GetTapeSet() {
  thread_local std::unique_ptr<tensorflow::gtl::CompactPointerSet<TFE_Py_Tape*>>
      tape_set;
  thread_local ThreadLocalDestructionMarker marker;
  if (!marker.alive) {
    // This thread is being destroyed. It is unsafe to access tape_set.
    return nullptr;
  }
  if (tape_set == nullptr) {
    tape_set.reset(new tensorflow::gtl::CompactPointerSet<TFE_Py_Tape*>);
  }
  return tape_set.get();
}