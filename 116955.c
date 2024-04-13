GetVariableWatcherSet() {
  thread_local std::unique_ptr<
      tensorflow::gtl::CompactPointerSet<TFE_Py_VariableWatcher*>>
      variable_watcher_set;
  thread_local ThreadLocalDestructionMarker marker;
  if (!marker.alive) {
    // This thread is being destroyed. It is unsafe to access
    // variable_watcher_set.
    return nullptr;
  }
  if (variable_watcher_set == nullptr) {
    variable_watcher_set.reset(
        new tensorflow::gtl::CompactPointerSet<TFE_Py_VariableWatcher*>);
  }
  return variable_watcher_set.get();
}